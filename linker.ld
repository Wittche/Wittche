/* Linker script for kernel */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /* Kernel loaded at 0x10000, but link addresses start at 0 */
    /* This way objcopy won't add padding to the binary */
    . = 0;

    /* Code section - will be loaded at 0x10000 by bootloader */
    .text 0x10000 : AT(0) {
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
