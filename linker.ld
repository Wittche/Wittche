/* Linker script for kernel */
OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB (0x10000) */
    . = 0x10000;

    /* Code section */
    .text : {
        *(.text)
    }

    /* Read-only data (string literals) - immediately after code */
    .rodata : {
        *(.rodata*)
    }

    /* Initialized data */
    .data : {
        *(.data)
    }

    /* Uninitialized data */
    .bss : {
        *(.bss)
    }

    /* Discard unwanted sections (debug info, exception handling, etc.) */
    /DISCARD/ : {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.note*)
        *(.comment)
        *(.debug*)
    }
}
