/* Linker script for kernel */
OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB (0x10000) */
    . = 0x10000;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
        *(.rodata*)
    }

    .bss : {
        *(.bss)
    }
}
