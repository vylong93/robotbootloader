/******************************************************************************
 *
 * Default Linker script for the Texas Instruments TM4C123GH6PM
 *
 * This is derived from revision 12626 of the TivaWare Library.
 *
 *****************************************************************************/

MEMORY
{
    FLASH (RX) : ORIGIN = 0x00000000, LENGTH = 0x00004000
    SRAM (WX)  : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

SECTIONS {

	PROVIDE (_text_run_address = 0x20000000);

    .text _text_run_address : AT (0x00000000)
    {
    	_text = .;
        KEEP(*(.isr_vector))
        CREATE_OBJECT_SYMBOLS
        KEEP (*(.text))
        *(.text.*)
        . = ALIGN(0x4);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        KEEP (*(.dtors))
        . = ALIGN(0x4);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        *(.init)
        *(.fini*)
        *(.rodata)
        *(.rodata*)
        *(.eh_frame)
        PROVIDE (__etext = .);
    	PROVIDE (_etext = .);
    	PROVIDE (etext = .);
    }

    .ARM.exidx _text_run_address + SIZEOF(.text) : AT (ADDR(.text) + SIZEOF(.text))
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    }

    .ARM.extab _text_run_address + SIZEOF(.text)  + SIZEOF(.ARM.exidx): AT (ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

	PROVIDE (_data_run_address = _text_run_address + SIZEOF(.text)  + SIZEOF(.ARM.exidx) + SIZEOF(.ARM.extab));

/*	PROVIDE (_data_run_address = 0x20000000);
*/

    .data _data_run_address : AT (ADDR(.ARM.extab) + SIZEOF(.ARM.extab))
    ALIGN (4) {
        __data_start__ = .;
        KEEP (*(.data))
        KEEP (*(.data*))
        . = ALIGN (4);
        __data_end__ = .;
    }

    .bss _data_run_address + SIZEOF(.data) : AT (ADDR(.data) + SIZEOF(.data))
    {
        __bss_start__ = .;
        PROVIDE (__bss_start__ = .);
        PROVIDE (_bss_start_ = .);
        PROVIDE (bss_start = .);
        PROVIDE (_bss_ = .);
        PROVIDE (_bss = .);
        *(.shbss)
        KEEP (*(.bss))
        *(.bss.*)
        *(COMMON)
        . = ALIGN (4);
        __bss_end__ = .;
        PROVIDE (_bss_end_ = .);
        PROVIDE (bss_end = .);
        PROVIDE (_ebss_ = .);
        PROVIDE (_ebss = .);
    }
}
