/* Linker script for kernel */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    /* Kernel loaded at 0x10000 physical, but binary starts at 0 */
    /* VMA (Virtual/Link Address) = 0x10000 - where code expects to run */
    /* LMA (Load Address) = 0 - where it appears in binary file */
    . = 0x10000;

    /* Code section */
    .text : AT(ADDR(.text) - 0x10000) {
        *(.text)
    }

    /* Read-only data (string literals) - immediately after code */
    .rodata : AT(ADDR(.rodata) - 0x10000) {
        *(.rodata*)
    }

    /* Initialized data */
    .data : AT(ADDR(.data) - 0x10000) {
        *(.data)
    }

    /* Uninitialized data */
    .bss : AT(ADDR(.bss) - 0x10000) {
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
