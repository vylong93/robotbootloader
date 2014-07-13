#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_gpio.h"
#include "inc/hw_adc.h"
#include "inc/hw_udma.h"
#include "inc/hw_timer.h"
#include "inc/hw_ssi.h"
#include "inc/hw_flash.h"
#include "driverlib/flash.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/ssi.h"
//#include "driverlib/rom.h"

#include "libnrf24l01/inc/TM4C123_nRF24L01.h"
#include "libnrf24l01/inc/nRF24L01.h"

#include "Config.h"
#include "boot_loader/bl_flash.h"

#define WAIT_TIME 100

#define BOOT_LOADER_START 0xAA

#define BOOT_LOADER_ACK1   0xAC
#define BOOT_LOADER_ACK2   0x0C
#define BOOT_LOADER_ACK3   0x48
#define BOOT_LOADER_ACK_LENGTH   3

#define LED_PORT_BASE           GPIO_PORTF_BASE
#define LED_PORT_CLOCK          SYSCTL_PERIPH_GPIOF
#define LED_ALL                 (GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define LED_RED                 GPIO_PIN_1
#define LED_BLUE                GPIO_PIN_2
#define LED_GREEN               GPIO_PIN_3

#define FIRST_PACKET_LENGTH 7

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
  BL_MISSING_PACKET
} BootLoaderEnum;

inline void initLED()
{
    SysCtlPeripheralEnable(LED_PORT_CLOCK);
    GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_RED | LED_GREEN | LED_BLUE);
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);
}

inline void initRfModule()
{
  RF24_InitTypeDef initRf24;
  initRf24.AddressWidth = RF24_ADRESS_WIDTH_3;
  initRf24.Channel = RF24_CHANNEL_0;
  initRf24.CrcBytes = RF24_CRC_2BYTES;
  initRf24.CrcState = RF24_CRC_EN;
  initRf24.RetransmitCount = RF24_RETRANS_COUNT15;
  initRf24.RetransmitDelay = RF24_RETRANS_DELAY_500u;
  initRf24.Speed = RF24_SPEED_1MBPS;
  initRf24.Power = RF24_POWER_0DBM;
  initRf24.Features = RF24_FEATURE_EN_DYNAMIC_PAYLOAD | RF24_FEATURE_EN_NO_ACK_COMMAND;
  initRf24.InterruptEnable = false;
  initRf24.LNAGainEnable = true;
  RF24_init(&initRf24);

  // Set payload pipe#0 dynamic
  RF24_PIPE_setPacketSize(RF24_PIPE0, RF24_PACKET_SIZE_DYNAMIC);

  // Open pipe#0 with Enhanced ShockBurst enabled for receiving Auto-ACKs
  RF24_PIPE_open(RF24_PIPE0, true);

  uint8_t addr[3] =  {0x1E, 0xAC, 0xC1}; // rx control
  RF24_TX_setAddress(addr);

  addr[0] = 0xDE;
  addr[1] = 0xAD;
  addr[2] = 0xBE;
  RF24_RX_setAddress(RF24_PIPE0, addr); // tx control

  RF24_RX_activate();
}

//*****************************************************************************
// Read data from the RF board.
// @param length: Buffer to store the length of received data
// @param readData: Buffer for received data
// @param waitTime: The time wait for new data.
// @return 1: if the data is available for processing. Otherwise, 0.
//*****************************************************************************
bool readDataRF(uint8_t * length, uint8_t * readData, uint32_t waitTime)
{
  uint32_t count = 0;
  uint8_t status;
  while(1)
  {
    if(GPIOPinRead(RF24_INT_PORT, RF24_INT_Pin) == 0)
    {
        // Get interrupt flag
        clearRfCSN();
        status = SPI_sendAndGetData(RF24_COMMAND_NOP);
        setRfCSN();
        status &= RF24_IRQ_MASK;
        status &= RF24_IRQ_RX;
        if(status != 0)
          break;
    }
    count++;
    if(count >= waitTime)
      return false;
  }

  // get pay load width
  clearRfCSN();
  status = SPI_sendAndGetData(RF24_COMMAND_R_RX_PL_WID);
  *length = SPI_sendAndGetData(RF24_COMMAND_NOP);
  setRfCSN();

  // get pay load data
  clearRfCSN();
  status = SPI_sendAndGetData(RF24_COMMAND_R_RX_PAYLOAD);
  for (count = 0; count < *length; count++)
  {
    *(readData + count) = SPI_sendAndGetData(RF24_COMMAND_NOP);
  }
  setRfCSN();

  // is RX empty
  status = RF24_readRegister(RF24_REG_FIFO_STATUS);
  if(status & RF24_RX_EMPTY)
  {
    // clear interrupt flag
    clearRfCSN();
    status = SPI_sendAndGetData(RF24_REG_STATUS | RF24_COMMAND_W_REGISTER);
    status &= 0x0F;
    status |= RF24_IRQ_RX;
    SPI_sendAndGetData(status);
    setRfCSN();
  }

  return true;
}

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

void Updater(void)
{
    uint32_t wait1ms = SysCtlClockGet() / 1000;

	uint8_t RF24_RX_buffer[32] = { 0 };
	uint8_t RF24_TX_buffer[3] = {BOOT_LOADER_ACK1, BOOT_LOADER_ACK2, BOOT_LOADER_ACK3};
	uint8_t rfDataLength;
	uint8_t firstPacketLength;

	uint32_t currentAddress;
	uint32_t transferAddress;
	uint32_t transferSize;
	uint32_t flashSize;

	uint32_t ui32Temp;
	uint16_t ui16checkSum;

	uint8_t isReceivedData;
	uint8_t byteCount;
	BootLoaderEnum status = BL_PROGRAM_SUCCESS;

        // Receive a packet
        isReceivedData = readDataRF(&rfDataLength, RF24_RX_buffer, WAIT_TIME*wait1ms);

        if(isReceivedData)
        {
            transferSize = convertByteToUINT32(&RF24_RX_buffer[0]);
            currentAddress = APP_START_ADDRESS;
            flashSize = transferSize + currentAddress;

            if(flashSize > MAX_FLASH_SIZE)
            {
                status = BL_TRANSFER_SIZE_TOO_LARGE;
            }

            // Clear the flash access interrupt.
            BL_FLASH_CL_ERR_FN_HOOK();

            // Erase the program space needed by the application.
            for(ui32Temp = APP_START_ADDRESS;
                ui32Temp < flashSize; ui32Temp += FLASH_PAGE_SIZE)
            {
                BL_FLASH_ERASE_FN_HOOK(ui32Temp);
            }

            if(BL_FLASH_ERROR_FN_HOOK())
            {
                status = BL_FLASH_ACCESS_FAILED;
            }
        }
        else
        {
          status = BL_RF24_TIMEOUT;
        }

        GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_BLUE);

        if(status != BL_PROGRAM_SUCCESS)
        {
			GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_ALL);
            // Errors occur during the erasing process wait here
            // until user reset the robot manually
			while(1)
            ;
        }

        // The programming Flash loop.
        // One data frame has two packets
        // First packet: Data length(1 byte) + check sum(2 bytes) + start address(3 bytes)
        // Second packet: Data
        while(1)
        {
           while(1)
           {
             //--------------------------First Packet Handle-------------------------------
             isReceivedData = readDataRF(&firstPacketLength, RF24_RX_buffer, WAIT_TIME*wait1ms);
             if(isReceivedData == 0)
             {
                 status = BL_RF24_TIMEOUT;
                 break;
             }

             byteCount = RF24_RX_buffer[0];
             // Check if there are any more bytes to receive.
             if(transferSize >= byteCount)
             {
                 // Get check sum
                 ui16checkSum = RF24_RX_buffer[1];
                 ui16checkSum <<= 8;
                 ui16checkSum |= RF24_RX_buffer[2];

                 // Calculate check sum
                 ui16checkSum += byteCount;
                 ui16checkSum += (RF24_RX_buffer[3] + RF24_RX_buffer[4]);
                 ui16checkSum += (RF24_RX_buffer[5] + RF24_RX_buffer[6]);

                 // Retrieve new program block address
                 transferAddress = convertByteToUINT32(&RF24_RX_buffer[3]);

                 //--------------------------Second Packet Handle-------------------------------
                 isReceivedData = readDataRF(&rfDataLength, RF24_RX_buffer, WAIT_TIME*wait1ms);
                 if(isReceivedData == 0)
                 {
                     status =  BL_RF24_TIMEOUT;
                     break;
                 }

                 // Calculate check sum
                 for(ui32Temp = 0; ui32Temp < byteCount; ui32Temp++)
                     ui16checkSum += RF24_RX_buffer[ui32Temp];

                 if(ui16checkSum != 0)
                 {
                     status = BL_WRONG_CHECKSUM;
                     break;
                 }

                 // Check other conditions after calculating checksum
                 // to avoid sending the jamming signal too soon when
                 // an error occurs
                 if(firstPacketLength != FIRST_PACKET_LENGTH)
                 {
                     status =  BL_INVALID_PACKET_LENGTH;
                     break;
                 }

                 // Make sure data length is a multiple of 4
                 if ((byteCount % 4) != 0)
                 {
                     status =  BL_INVALID_DATA_LENGTH;
                     break;
                 }

                 if(currentAddress < transferAddress)
                 {
                     status =  BL_MISSING_PACKET;
                     break;
                 }

                 // Is this a new packet?
                 if(currentAddress == transferAddress)
                 {
                     // Clear the flash access interrupt.
                     BL_FLASH_CL_ERR_FN_HOOK();
                     // Write this block of data to the flash
                     BL_FLASH_PROGRAM_FN_HOOK(transferAddress,
                                              (uint8_t *) &RF24_RX_buffer[0],
                                              rfDataLength);
                     if(BL_FLASH_ERROR_FN_HOOK())
                     {
                         status = BL_FLASH_ACCESS_FAILED;
                         break;
                     }
                     currentAddress += byteCount;
                     transferSize -= byteCount;
                 }
                   
				 //IMPORTANT!: ONLY ONE robot must be built with the command below uncommented
                 // Wait longer than 130us to wait control board go into RX mode
                 // or if there is any jamming singal.
                 // TODO: The real wait time should be calibrated throught testing
                 GPIOPinWrite(LED_PORT_BASE, LED_RED, LED_RED);
				 RF24_TX_activate();
                 rfDelayLoop(DELAY_CYCLES_130US*3);
                 // Send an ack signal back to the control board if it is the "chosen one"
                 RF24_TX_writePayloadNoAck(BOOT_LOADER_ACK_LENGTH, RF24_TX_buffer);
                 RF24_TX_pulseTransmit();
                 RF24_RX_activate();
				 GPIOPinWrite(LED_PORT_BASE, LED_RED, 0);
				 
                 // Did we finish updating application code?
                 if(transferSize == 0)
                 {
                     GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);
                     status = BL_PROGRAM_SUCCESS;

                     // Reset to jump to app code
                     HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
                                          NVIC_APINT_SYSRESETREQ);

                     // An infinite loop is placed here to prevent error
                     // if the device is not reset
                     while(1)
                       ;
                 }

             }
             else
             {
                 status = BL_DATA_OVERFLOWED;
                 break;
             }
           }

		   GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_GREEN);
		   while(1);
		   // Some errors occur -> send a jamming signal
           RF24_TX_activate();
           RF24_TX_sendJammingSignal();
           rfDelayLoop(DELAY_CYCLES_1MS5);
           RF24_TX_stopJammingSignal();

           // Go back to the RX mode to receive a new data frame
           RF24_RX_activate();
        }
}

//*****************************************************************************
//! Checks if an update is needed or is being requested.
//! If so, enter bootloader mode
//!
//! \return Returns a non-zero value if an update is needed or is being
//! requested and zero otherwise.
//*****************************************************************************
void CheckForceUpdate(void)
{
    FPULazyStackingEnable();
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_INT);
    initRfModule();

    initLED();
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, LED_RED);

    uint8_t RF24_RX_buffer[32];
    uint8_t rfDataLength;

    uint8_t isReceivedData = readDataRF(&rfDataLength, RF24_RX_buffer,
                                        WAIT_TIME*(SysCtlClockGet()/1000));
    GPIOPinWrite(LED_PORT_BASE, LED_ALL, 0);
    if(isReceivedData == false)
      return;

    // If the received data is not a valid START_BOOT_LOADER
    // command then jump to application code
    if( (RF24_RX_buffer[0] != BOOT_LOADER_START) ||
        (rfDataLength != 1))
    {
        //!!!!!!!!!!!!!!Importance!!!!!!!!!!!!!!!!!!!!!!!
        // Reset to jump to app code instead of returning
        // to the asm and branch to CallApplication because
        // the link register (lr) points to the wrong return
        // address after reading RF data. The reason is still
        // unknown.
        HWREG(NVIC_APINT) = (NVIC_APINT_VECTKEY |
                             NVIC_APINT_SYSRESETREQ);

    }

    Updater();
}

