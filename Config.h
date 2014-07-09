//*****************************************************************************
// The starting address of the application.  This must be a multiple of 1024
// bytes (making it aligned to a page boundary).  A vector table is expected at
// this location, and the perceived validity of the vector table (stack located
// in SRAM, reset vector located in flash) is used as an indication of the
// validity of the application image.
//*****************************************************************************
#define APP_START_ADDRESS       0x00002000

//*****************************************************************************
// The address at which the application locates its exception vector table.
// This must be a multiple of 1KB (making it aligned to a page boundary).
// Typically, an application will start with its vector table and this value
// will default to APP_START_ADDRESS.  This option is provided to cater for
// applications which run from external memory which may not be accessible by
// the NVIC (the vector table offset register is only 30 bits long).
//*****************************************************************************
#define VTABLE_START_ADDRESS    0x00002000

//*****************************************************************************
// The size of a single, erasable page in the flash.  This must be a power
// of 2.  The default value of 1KB represents the page size for the internal
// flash on all Tiva MCUs and this value should only be overridden if
// configuring a boot loader to access external flash devices with a page size
// different from this.
//*****************************************************************************
#define FLASH_PAGE_SIZE         0x00000400

//*****************************************************************************
// The flash size of the MCU
//*****************************************************************************
#define MAX_FLASH_SIZE         0x00040000

//*****************************************************************************
// The number of words of stack space to reserve for the boot loader.
//*****************************************************************************
#define STACK_SIZE              256
