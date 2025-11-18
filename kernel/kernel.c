/**
 * Wittche Operating System - Main Kernel
 *
 * This is the main kernel entry point for the Wittche OS.
 * It initializes all system components and starts the shell.
 */

#include "../include/types.h"
#include "../include/screen.h"
#include "../include/idt.h"
#include "../include/keyboard.h"
#include "../include/shell.h"

/**
 * Main kernel entry point
 * Called from kernel_entry.asm after bootloader hands control
 */
void kernel_main(void) {
    // Initialize screen/VGA driver
    screen_init();

    // Display boot messages
    screen_write_color("[BOOT] ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("Wittche OS kernel loaded at ");
    screen_write_hex(0x10000);
    screen_write("\n");

    // Initialize Interrupt Descriptor Table
    idt_init();

    // Initialize keyboard driver
    keyboard_init();

    // Display welcome banner
    shell_display_banner();

    // Start the shell (never returns)
    shell_run();

    // Should never reach here
    screen_write_color("\n[KERNEL PANIC] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
    screen_write("Shell unexpectedly terminated!\n");

    // Halt the system
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
