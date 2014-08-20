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
#include "bl_config.h"
#include "boot_loader/bl_flash.h"
#include "boot_loader/bl_hooks.h"
#ifdef CHECK_CRC
#include "boot_loader/bl_crc32.h"
#endif

#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#include "libnrf24l01/inc/nRF24L01.h"

extern void StartApplication(void);

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

#define BSL_WAIT_TIME 			100
#define BSL_START_COMMAND		0xAA

#define BSL_START_PACKET_LENGTH		6       // <0xAA><transferSize><checksum>
// BSL Start Packet Offset
#define BOOTLOADER_COMMAND_OFFSET       0       // must be 0xAA
#define TRANSFER_SIZE_OFFSET            1       // (BOOTLOADER_COMMAND_OFFSET + 1)
#define CHECKSUM_OFFSET                 5       // (TRANSFER_SIZE_OFFSET + 4)

#define BSL_PACKET_DATA_LENGTH          16
#define BSL_PACKET_FULL_LENGTH          23      // <program address><byte count><data[0]...data[n]><checksum>
// BSL Program Packet Offset
#define BSL_PACKET_ADDRESS_OFFSET       0
#define BSL_PACKET_BYTECOUNT_OFFSET     4       // (BSL_PACKET_ADDRESS_OFFSET + 4)
#define BSL_PACKET_DATA_OFFSET          5       // (BSL_PACKET_BYTECOUNT_OFFSET + 1)
#define BSL_PACKET_CHECKSUM_OFFSET      21      // (BSL_PACKET_DATA_OFFSET + BSL_PACKET_DATA_LENGTH)
#define BSL_PACKET_CHECKSUM 1

#define LED_PORT_BASE           GPIO_PORTF_BASE
#define LED_PORT_CLOCK          SYSCTL_PERIPH_GPIOF
#define LED_ALL                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LED_RED                 GPIO_PIN_1
#define LED_BLUE                GPIO_PIN_2
#define LED_GREEN               GPIO_PIN_3

uint32_t g_ui32TransferSize;

uint32_t g_1msCycles;

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

uint8_t waitForRfPacket(uint32_t ui32TimeOut, uint8_t *pui8RfBuffer)
{
  uint32_t ui32BootTime = 0;

  while (ui32BootTime < ui32TimeOut)
  {
    if (ROM_GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
      if (RF24_getIrqFlag(RF24_IRQ_RX))
      {
        ui8RxLength = RF24_RX_getPayloadWidth();
        RF24_RX_getPayloadData(ui8RxLength, pui8RfBuffer);
        RF24_clearIrqFlag(RF24_IRQ_RX);
        return ui8RxLength;
      }
      else
      {
        RF24_clearIrqFlag(RF24_IRQ_MASK);
      }
    }
    ui32BootTime++;
  }
  return 0;
}

// This function will be called immediately after the boot loader
// code relocation completes in SRAM
void MyHwInitFunc(void)
{
  // Init system clock and RF configuration prepare for CheckForceUpdate
  ROM_FPULazyStackingEnable();
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_INT);  // 50MHz

  g_1msCycles = ROM_SysCtlClockGet() / 1000;

  //----------------------- initRfModule() BEGIN -----------------------
  RF24_InitTypeDef initRf24;
  initRf24.AddressWidth = RF24_ADRESS_WIDTH_3;
  initRf24.Channel = RF24_CHANNEL_0;
  initRf24.CrcBytes = RF24_CRC_2BYTES;
  initRf24.CrcState = RF24_CRC_EN;
  initRf24.RetransmitCount = RF24_RETRANS_COUNT15;
  initRf24.RetransmitDelay = RF24_RETRANS_DELAY_4000u;
  initRf24.Speed = RF24_SPEED_1MBPS;
  initRf24.Power = RF24_POWER_0DBM;
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD | RF24_FEATURE_EN_NO_ACK_COMMAND;
  initRf24.InterruptEnable = false;
  initRf24.LNAGainEnable = false;
  RF24_init(&initRf24);

  // Set payload pipe#0 dynamic
  RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);

  // Open pipe#0 with Enhanced ShockBurst enabled for receiving Auto-ACKs
  RF24_PIPE_open(RF24_PIPE0, true);

  uint8_t addr[3] =
  { 0x0E, 0xAC, 0xC1 };  // rx control
  RF24_TX_setAddress(addr);

  addr[0] = 0xDE;
  addr[1] = 0xAD;
  addr[2] = 0xBE;
  RF24_RX_setAddress(RF24_PIPE0, addr);  // tx control

  RF24_RX_activate();
  //------------------------ initRfModule() END ------------------------

  //----------------------- initLED() BEGIN -----------------------
  ROM_SysCtlPeripheralEnable(LED_PORT_CLOCK);
  ROM_GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_RED | LED_GREEN | LED_BLUE);
  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);
  //------------------------ initLED() END ------------------------
}

int main(void)
{
  // where the return code is 0 if the boot loader should boot the existing
  // image (if found) or non-zero to indicate that the boot loader should retain
  // control and wait for a new firmware image to be downloaded.

  //return 1;	/* EnterBootLoader */
  //return 0; 	/* CallApplication */

  uint8_t ui8CheckSum;
  uint32_t ui32TimeOut = BSL_WAIT_TIME * g_1msCycles;

  ui8RxLength = waitForRfPacket(ui32TimeOut, pui8RfBuffer);

  if (ui8RxLength == BSL_START_PACKET_LENGTH)
  {
    // checksum received packet - Optimized
    ui8CheckSum = pui8RfBuffer[CHECKSUM_OFFSET];
    ui8CheckSum += pui8RfBuffer[0] + pui8RfBuffer[1] + pui8RfBuffer[2] + pui8RfBuffer[3] + pui8RfBuffer[4];

    // check for 0xAA command and get NumberOfPacket, TO2 delay
    if (pui8RfBuffer[BOOTLOADER_COMMAND_OFFSET] == BSL_START_COMMAND && ui8CheckSum == 0x00)
    {
      g_ui32TransferSize = convertByteToUINT32(pui8RfBuffer + TRANSFER_SIZE_OFFSET);

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

  ROM_SSIDisable(RF24_SPI);
  ROM_SysCtlPeripheralDisable(RF24_SPI_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(RF24_INT_PORT_CLOCK);
  ROM_SysCtlPeripheralDisable(RF24_SPI_CLOCK);
  ROM_SysCtlPeripheralDisable(LED_PORT_CLOCK);
  return 0; /* CallApplication */
}

void ConfigureDevice(void)
{
  // addition MCU configuration if need after EnterBootLoader
  uint8_t addr[3];
  addr[2] = 0xC1;
  addr[1] = 0xAC;
  addr[0] = 0x02;
  RF24_TX_setAddress(addr);

  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);
  RF24_RX_activate();
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
  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);

  RF24_TX_activate();

  ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);

  // ROM_SysCtlDelay(833333); // 250000/3 ~5ms
  ROM_SysCtlDelay((g_1msCycles << 1) - 20000);

  clearRfCSN();
  SPI_sendAndGetData(RF24_COMMAND_W_TX_PAYLOAD_NOACK);
  SPI_sendAndGetData(0x00);
  setRfCSN();
  RF24_TX_pulseTransmit();

  while (ROM_GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) != 0)
                          ;
  RF24_clearIrqFlag(RF24_IRQ_TX);
  
  RF24_RX_activate();
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

    ui8RxLength = waitForRfPacket(g_1msCycles, pui8RfBuffer); // wait for 30ms

    if (ui8RxLength != 0)
    {
      ui32ReceivedAddress = convertByteToUINT32(pui8RfBuffer + BSL_PACKET_ADDRESS_OFFSET);
      ui32ByteCount = pui8RfBuffer[BSL_PACKET_BYTECOUNT_OFFSET];
      // Is this a new packet?
      if (ui32ReceivedAddress == ui32ExpectedAddress)
      {

#ifdef BSL_PACKET_CHECKSUM
        int i;
        ui16Checksum = pui8RfBuffer[ui8RxLength - 2] << 8;
        ui16Checksum |= pui8RfBuffer[ui8RxLength - 1];
        for (i = 0; i < (ui8RxLength - 2); i++)
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
        BL_FLASH_PROGRAM_FN_HOOK(ui32ReceivedAddress, (pui8RfBuffer + BSL_PACKET_DATA_OFFSET), ui32ByteCount);

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
        NACK();
        continue;
      }
      else
      {
       // RF24_TX_activate();

       // ROM_GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);

       // ROM_SysCtlDelay(g_1msCycles << 1);

       // //RF24_TX_writePayloadNoAck(1, (unsigned char*) (&jamming));
       // clearRfCSN();
       // SPI_sendAndGetData(RF24_COMMAND_W_TX_PAYLOAD_NOACK);
       // SPI_sendAndGetData(0xAC);
       // SPI_sendAndGetData(0x0C);
       // SPI_sendAndGetData(0x48);
       // setRfCSN();

       // RF24_TX_pulseTransmit();

       // while (ROM_GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) != 0)
                   // ;
       // RF24_clearIrqFlag(RF24_IRQ_TX);

       // RF24_RX_activate();

        // Did we finish updating application code?
        if (ui32ExpectedAddress == ui32ProgramEndAddress)
        {
          ExitBootloader();
        }
      }
    }
    else
    {
      // time out
      NACK();
      continue;
    }
  }
}
