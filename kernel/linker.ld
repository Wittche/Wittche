/* AuroraOS Kernel Linker Script
 * For Multiboot2 64-bit kernel
 * Kernel loaded at 1MB (0x100000) physical address
 */
OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    /* Kernel starts at 1MB physical address */
    . = 0x100000;

    /* Multiboot2 header MUST be at the start */
    .boot ALIGN(8) :
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data - 4KB aligned */
    .rodata ALIGN(4K) :
    {
        *(.rodata*)
    }

    /* Initialized data - 4KB aligned */
    .data ALIGN(4K) :
    {
        *(.data)
    }

    /* Uninitialized data (BSS) - 4KB aligned */
    .bss ALIGN(4K) :
    {
        *(.bss)
        *(COMMON)
    }

    /* Discard debug and unneeded sections */
    /DISCARD/ : {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.note.GNU-stack)
        *(.comment)
    }

    _kernel_end = .;
}
