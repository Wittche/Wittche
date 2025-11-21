/* Linker script for kernel */
OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    /* Kernel loaded at 0x10000 by bootloader */
    . = 0x10000;

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}
