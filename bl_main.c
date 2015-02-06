//*****************************************************************************
//
// bl_main.c - The file holds the main control loop of the boot loader.
//
// Copyright (c) 2006-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the Tiva Firmware Development Package.
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_flash.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_ssi.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bl_config.h"
#include "boot_loader/bl_flash.h"
#include "boot_loader/bl_hooks.h"
#ifdef CHECK_CRC
#include "boot_loader/bl_crc32.h"
#endif

#ifdef RF_USE_CC2500
#include "libcc2500/inc/TM4C123_CC2500.h"
#include "libcc2500/inc/cc2500.h"
#endif

#ifdef RF_USE_nRF24L01
#include "libnrf24l01/inc/nRF24L01.h"
#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#endif

extern void StartApplication(void);

void MCU_RF_IRQ_handler(void);
void MCU_RF_IRQ_handler(void){}

//*****************************************************************************
//
// Make sure that the application start address falls on a flash page boundary
//
//*****************************************************************************
#if (APP_START_ADDRESS & (FLASH_PAGE_SIZE - 1))
#error ERROR: APP_START_ADDRESS must be a multiple of FLASH_PAGE_SIZE bytes!
#endif

//*****************************************************************************
//
// Make sure that the flash reserved space is a multiple of flash pages.
//
//*****************************************************************************
#if (FLASH_RSVD_SPACE & (FLASH_PAGE_SIZE - 1))
#error ERROR: FLASH_RSVD_SPACE must be a multiple of FLASH_PAGE_SIZE bytes!
#endif


typedef enum
{
	BL_PROGRAM_SUCCESS,
	BL_INVALID_ADDRESS,
	BL_FLASH_ACCESS_FAILED,
	BL_TRANSFER_SIZE_TOO_LARGE,
	BL_INVALID_DATA_LENGTH,
	BL_INVALID_PACKET_LENGTH,
	BL_RF24_TIMEOUT,
	BL_DATA_OVERFLOWED,
	BL_WRONG_CHECKSUM,
	BL_UNKNOWN_COMMAND,
	BL_MISSING_PACKET,
	BL_JAMMING,
} BootLoaderEnum;

#define MAX_FLASH_SIZE        		0x00040000

// Non-Interrtup Timer
#define DELAY_TIMER_CLOCK_NON_INT	SYSCTL_PERIPH_TIMER0
#define DELAY_TIMER_BASE_NON_INT	TIMER0_BASE

#define BSL_WAIT_TIME_US 			3000000	 // unit us
#define BSL_START_COMMAND			0xAA

#define BSL_PACKET_LENGTH_IDX		0
#define BSL_START_PACKET_LENGTH		10       // <0xAA><transferSize><waitTime><checksum>
// BSL Start Packet index
#define BOOTLOADER_COMMAND_IDX       1       // must be 0xAA (BSL_PACKET_LENGTH_IDX + 1)
#define TRANSFER_SIZE_IDX            2       // (BOOTLOADER_COMMAND_IDX + 1)
#define TRANSFER_SIZE_IDX_UPPER      3       // (TRANSFER_SIZE_IDX + 1)
#define TRANSFER_SIZE_IDX_HIGH       4       // (TRANSFER_SIZE_IDX_UPPER + 1)
#define TRANSFER_SIZE_IDX_LOW        5       // (TRANSFER_SIZE_IDX_HIGH + 1)
#define WAIT_TIME_IDX            	 6       // (TRANSFER_SIZE_IDX_LOW + 1)
#define WAIT_TIME_IDX_UPPER      	 7       // (WAIT_TIME_IDX + 1)
#define WAIT_TIME_IDX_HIGH       	 8       // (WAIT_TIME_IDX_UPPER + 1)
#define WAIT_TIME_IDX_LOW        	 9       // (WAIT_TIME_IDX_HIGH + 1)
#define CHECKSUM_IDX                 10      // (WAIT_TIME_IDX_LOW + 4)

#define BSL_PACKET_DATA_LENGTH       32
#define BSL_PACKET_FULL_LENGTH       40      // <program address><byte count><data[0]...data[n]><checksum>

// BSL Program Packet index
#define BSL_PACKET_ADDRESS_IDX       1		 // (BSL_PACKET_LENGTH_IDX + 1)
#define BSL_PACKET_BYTECOUNT_IDX     5       // (BSL_PACKET_ADDRESS_IDX + 4)
#define BSL_PACKET_DATA_IDX          6       // (BSL_PACKET_BYTECOUNT_IDX + 1)

#define BSL_PACKET_CHECKSUM

#define BSL_PAKCET_HEADER_LENGTH	7	// Smallest valid packet: <4b Addr><1b cout><1b data><2b checksum>
#define BSL_NACK_PACKET_LENGTH		2	// must be less than BSL_PAKCET_HEADER_LENGTH

#define LED_PORT_BASE           GPIO_PORTF_BASE
#define LED_PORT_CLOCK          SYSCTL_PERIPH_GPIOF
#define LED_ALL                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LED_RED                 GPIO_PIN_1
#define LED_BLUE                GPIO_PIN_2
#define LED_GREEN               GPIO_PIN_3

uint32_t g_ui32TransferSize;

uint32_t g_ui32WaitingPeriodUs;

// update firmware process
uint8_t ui8RxLength;
uint8_t pui8RfBuffer[BSL_PACKET_FULL_LENGTH];

uint32_t convertByteToUINT32(uint8_t data[])
{
  uint32_t result = data[0];
  result <<= 8;
  result |= data[1];
  result <<= 8;
  result |= data[2];
  result <<= 8;
  result |= data[3];
  return result;
}

uint8_t waitForRfPacket(uint32_t ui32TimeOutUs, uint8_t *pui8RfBuffer)
{
  e_RxStatus eStatus;
  uint32_t ui32Status = 0;

  ui32TimeOutUs = ROM_SysCtlClockGet() / 1000000 * ui32TimeOutUs;

  ROM_TimerLoadSet(DELAY_TIMER_BASE_NON_INT, TIMER_A, ui32TimeOutUs);
  ROM_TimerIntClear(DELAY_TIMER_BASE_NON_INT, TIMER_TIMA_TIMEOUT);
  ROM_TimerEnable(DELAY_TIMER_BASE_NON_INT, TIMER_A);

  while (true)
  {
	if ((GPIOIntStatus(RF24_INT_PORT, false) & RF24_INT_Pin) == RF24_INT_Pin)  // MCU_RF_IsInterruptPinAsserted()
	{
	  GPIOIntClear(RF24_INT_PORT, RF24_INT_Pin); // MCU_RF_ClearIntFlag();

	  eStatus = RfReceivePacket(pui8RfBuffer);

	  if(eStatus == RX_STATUS_SUCCESS)
	  {
		  ui8RxLength = pui8RfBuffer[BSL_PACKET_LENGTH_IDX];
		  return ui8RxLength;
	  }
	}

	ui32Status = ROM_TimerIntStatus(DELAY_TIMER_BASE_NON_INT, false);

    if (ui32Status & TIMER_TIMA_TIMEOUT)
    {
	  ROM_TimerIntClear(DELAY_TIMER_BASE_NON_INT, TIMER_TIMA_TIMEOUT);
	   break;
    }
  }

  return 0;
}

// This function will be called immediately after the boot loader
// code relocation completes in SRAM
void MyHwInitFunc(void)
{
  // Init system clock and RF configuration prepare for CheckForceUpdate
  ROM_FPULazyStackingEnable();
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_INT);  // 50MHz internal
//  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  initRfModule(false);

  //----------------------- initLED() BEGIN -----------------------
  ROM_SysCtlPeripheralEnable(LED_PORT_CLOCK);
  ROM_GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_RED | LED_GREEN | LED_BLUE);
  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
  //------------------------ initLED() END ------------------------

  // Non-interrupt timer delay
  ROM_SysCtlPeripheralEnable(DELAY_TIMER_CLOCK_NON_INT);
  TimerClockSourceSet(DELAY_TIMER_BASE_NON_INT, TIMER_CLOCK_SYSTEM);
  ROM_TimerConfigure(DELAY_TIMER_BASE_NON_INT, TIMER_CFG_ONE_SHOT);

  ROM_TimerIntEnable(DELAY_TIMER_BASE_NON_INT, TIMER_TIMA_TIMEOUT);
  ROM_TimerIntClear(DELAY_TIMER_BASE_NON_INT, TIMER_TIMA_TIMEOUT);
}

int main(void) // CheckForceUpdate
{
  // where the return code is 0 if the boot loader should boot the existing
  // image (if found) or non-zero to indicate that the boot loader should retain
  // control and wait for a new firmware image to be downloaded.

  //return 1;	/* EnterBootLoader */
  //return 0; 	/* CallApplication */

  uint8_t ui8CheckSum;

  ui8RxLength = waitForRfPacket(BSL_WAIT_TIME_US, pui8RfBuffer);

  if (ui8RxLength == BSL_START_PACKET_LENGTH)
  {
    // checksum received packet - Optimized
	ui8CheckSum = pui8RfBuffer[CHECKSUM_IDX];
	ui8CheckSum += pui8RfBuffer[BOOTLOADER_COMMAND_IDX]
				 + pui8RfBuffer[TRANSFER_SIZE_IDX] + pui8RfBuffer[TRANSFER_SIZE_IDX_UPPER]
				 + pui8RfBuffer[TRANSFER_SIZE_IDX_HIGH] + pui8RfBuffer[TRANSFER_SIZE_IDX_LOW]
				 + pui8RfBuffer[WAIT_TIME_IDX] + pui8RfBuffer[WAIT_TIME_IDX_UPPER]
				 + pui8RfBuffer[WAIT_TIME_IDX_HIGH] + pui8RfBuffer[WAIT_TIME_IDX_LOW];

    // check for 0xAA command and get NumberOfPacket, TO2 delay
	if (pui8RfBuffer[BOOTLOADER_COMMAND_IDX] == BSL_START_COMMAND && ui8CheckSum == 0x00)
    {
      g_ui32TransferSize = convertByteToUINT32(&pui8RfBuffer[TRANSFER_SIZE_IDX]);
      g_ui32WaitingPeriodUs = convertByteToUINT32(&pui8RfBuffer[WAIT_TIME_IDX]);
      return 1; /* EnterBootLoader */
    }
  }
  if (ui8RxLength != 0)
  {
    //
    // In case this ever does return and the boot loader is still
    // intact, simply reset the device.
    //
    HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY | NVIC_APINT_SYSRESETREQ);

    //
    // The microcontroller should have reset, so this should
    // never be reached.  Just in case, loop forever.
    //
    while (1);
  }

  ROM_SysCtlPeripheralDisable(DELAY_TIMER_CLOCK_NON_INT);
  ROM_SysCtlPeripheralDisable(LED_PORT_CLOCK);

#ifdef RF_USE_CC2500
  ROM_SSIDisable(CC2500_SPI);
  ROM_SysCtlPeripheralDisable(CC2500_SPI_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(CC2500_INT_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(CC2500_SPI_CLOCK);
#endif
#ifdef RF_USE_nRF24L01
  ROM_SSIDisable(RF24_SPI);
  ROM_SysCtlPeripheralDisable(RF24_SPI_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(RF24_INT_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(RF24_SPI_CLOCK);
#endif

  return 0; /* CallApplication */
}

void ConfigureDevice(void)
{
  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);
}

inline void ExitBootloader(void)
{
  //
  // In case this ever does return and the boot loader is still
  // intact, simply reset the device.
  //
  HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
  NVIC_APINT_SYSRESETREQ);

  //
  // The microcontroller should have reset, so this should
  // never be reached.  Just in case, loop forever.
  //
  while (1);
}

void NACK(void)
{
  pui8RfBuffer[0] = BSL_NACK_PACKET_LENGTH - 1;
  pui8RfBuffer[1] = 0xFA;

  //  Delay 1ms
  ROM_SysCtlDelay(ROM_SysCtlClockGet() / 1000);

  ROM_GPIOPinWrite(LED_PORT_BASE, LED_GREEN, LED_GREEN);

#ifdef RF_USE_CC2500
  TI_CC_Strobe(TI_CCxxx0_SIDLE);
  RfWriteBurstReg(TI_CCxxx0_TXFIFO, pui8RfBuffer, BSL_NACK_PACKET_LENGTH); // Write TX data
  TI_CC_Strobe(TI_CCxxx0_STX);

  while (!ROM_GPIOPinRead(CC2500_INT_PORT, CC2500_INT_Pin));
  while (ROM_GPIOPinRead(CC2500_INT_PORT, CC2500_INT_Pin));

  GPIOIntClear(CC2500_INT_PORT, CC2500_INT_Pin);

  TI_CC_Strobe(TI_CCxxx0_SRX);      // Initialize CCxxxx in RX mode.
#endif
#ifdef RF_USE_nRF24L01
  	RfSendPacket(pui8RfBuffer);
#endif

  ROM_GPIOPinWrite(LED_PORT_BASE, LED_GREEN, 0);
}

void Updater(void)
{
  uint32_t ui32Temp;
  uint32_t ui32ReceivedAddress;
  uint32_t ui32ExpectedAddress;
  uint32_t ui32ProgramEndAddress;
  uint32_t ui32ByteCount;
  BootLoaderEnum eBootStatus = BL_PROGRAM_SUCCESS;

#ifdef BSL_PACKET_CHECKSUM
  uint16_t ui16Checksum;
#endif

  ui32ProgramEndAddress = g_ui32TransferSize + APP_START_ADDRESS;

  if (ui32ProgramEndAddress > MAX_FLASH_SIZE)
  {
    ExitBootloader();   // eBootStatus = BL_TRANSFER_SIZE_TOO_LARGE;
  }

  // Clear the flash access interrupt.
  BL_FLASH_CL_ERR_FN_HOOK();

  // Erase the program space needed by the application.
  //  for (ui32Temp = APP_START_ADDRESS; ui32Temp < g_ui32TransferSize; ui32Temp += FLASH_PAGE_SIZE)
  for (ui32Temp = APP_START_ADDRESS; ui32Temp < MAX_FLASH_SIZE; ui32Temp += FLASH_PAGE_SIZE)
  {
    BL_FLASH_ERASE_FN_HOOK(ui32Temp);
  }

  if (BL_FLASH_ERROR_FN_HOOK())
  {
    ExitBootloader();
  }

  ui32ExpectedAddress = APP_START_ADDRESS;

  // The programming Flash loop.
  while (ui32ExpectedAddress < ui32ProgramEndAddress)
  {
    ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

    ui8RxLength = waitForRfPacket(g_ui32WaitingPeriodUs, pui8RfBuffer);

    if (ui8RxLength > BSL_PAKCET_HEADER_LENGTH && ui8RxLength < BSL_PACKET_FULL_LENGTH)
    {
      ui32ReceivedAddress = convertByteToUINT32(&pui8RfBuffer[BSL_PACKET_ADDRESS_IDX]);
      ui32ByteCount = pui8RfBuffer[BSL_PACKET_BYTECOUNT_IDX];

      // Is this a new packet?
      if (ui32ReceivedAddress == ui32ExpectedAddress)
      {

#ifdef BSL_PACKET_CHECKSUM
        int i;
        ui16Checksum = pui8RfBuffer[ui8RxLength - 1] << 8;
        ui16Checksum |= pui8RfBuffer[ui8RxLength];
        for (i = 1; i < (ui8RxLength - 1); i++)
            ui16Checksum += pui8RfBuffer[i];
        if (ui16Checksum != 0)
        {
          eBootStatus = BL_WRONG_CHECKSUM;
          NACK();
          continue;
        }
#endif

        // Make sure data length is a multiple of 4
        if ((ui32ByteCount % 4) != 0)
        {
          eBootStatus = BL_INVALID_DATA_LENGTH;
          NACK();
          continue;
        }

        // Clear the flash access interrupt.
        BL_FLASH_CL_ERR_FN_HOOK();

        // Write this block of data to the flash
        BL_FLASH_PROGRAM_FN_HOOK(ui32ReceivedAddress, &pui8RfBuffer[BSL_PACKET_DATA_IDX], ui32ByteCount);

        if (BL_FLASH_ERROR_FN_HOOK())
        {
          eBootStatus = BL_FLASH_ACCESS_FAILED;
          NACK();
          continue;
        }

        ui32ExpectedAddress += ui32ByteCount;

        eBootStatus = BL_PROGRAM_SUCCESS;
      }
      
      if (ui32ReceivedAddress > ui32ExpectedAddress)
      {
    	ROM_GPIOPinWrite(LED_PORT_BASE, LED_RED, LED_RED);
        NACK();
        continue;
      }
      else
      {
        // Did we finish updating application code?
        if (ui32ExpectedAddress == ui32ProgramEndAddress)
        {
          ExitBootloader();
        }
      }
    }
    else
    {
      if (ui8RxLength == 0)	// time out
      {
    	  //ROM_GPIOPinWrite(LED_PORT_BASE, LED_RED, LED_RED);
//    	  NACK();
      }
      continue;
    }
  }
}
