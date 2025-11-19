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
#include "../include/timer.h"
#include "../include/shell.h"

/**
 * Main kernel entry point
 * Called from kernel_entry.asm after bootloader hands control
 */
void kernel_main(void) {
    // Initialize screen/VGA driver first
    screen_init();

    // Display boot messages
    screen_write_color("[BOOT] ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("Wittche OS kernel loaded at ");
    screen_write_hex(0x10000);
    screen_write("\n");

    // Initialize Interrupt Descriptor Table
    idt_init();

    // Initialize timer (PIT)
    timer_init();

    // Initialize keyboard driver
    keyboard_init();

    // Debug: Check if we reach here
    screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write("About to display banner...\n");

    // Display welcome banner
    shell_display_banner();

    // Debug: Check if banner displayed
    screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write("Banner displayed, starting shell...\n");

    // CRITICAL: Direct VGA write BEFORE shell_run() call
    __asm__ __volatile__("cli");
    volatile unsigned short *vga = (unsigned short *)0xB8000;
    const char *msg1 = "<<<BEFORE shell_run()>>>";
    for (int i = 0; msg1[i] != '\0'; i++) {
        vga[320 + i] = 0x2F00 | msg1[i];  // White on green
    }
    __asm__ __volatile__("sti");

    // Start the shell (never returns)
    shell_run();

    // CRITICAL: If we reach here, shell_run() returned (should never happen!)
    __asm__ __volatile__("cli");
    const char *msg2 = "!!!AFTER shell_run()!!!";
    for (int i = 0; msg2[i] != '\0'; i++) {
        vga[480 + i] = 0xCF00 | msg2[i];  // White on bright red
    }
    __asm__ __volatile__("sti");

    // Should never reach here
    screen_write_color("\n[KERNEL PANIC] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
    screen_write("Shell unexpectedly terminated!\n");

    // Halt the system
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
