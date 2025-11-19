/**
 * Wittche Operating System - Main Kernel
 *
 * This is the main kernel entry point for the Wittche OS.
 * It initializes all system components and starts the shell.
 */

#include "../include/types.h"
#include "../include/screen.h"
#include "../include/idt.h"
#include "../include/pmm.h"
#include "../include/heap.h"
#include "../include/paging.h"
#include "../include/process.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/shell.h"

/**
 * Test process A - prints message periodically
 */
void test_process_a(void) {
    int count = 0;
    while (count < 5) {
        screen_write_color("[Process A] ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
        screen_write("Running iteration ");
        screen_write_dec(count);
        screen_write("\n");
        count++;

        // Busy wait
        for (volatile int i = 0; i < 10000000; i++);
    }

    screen_write_color("[Process A] ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    screen_write("Finished!\n");

    // Process terminates
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

/**
 * Test process B - prints different message
 */
void test_process_b(void) {
    int count = 0;
    while (count < 5) {
        screen_write_color("[Process B] ", MAKE_COLOR(COLOR_LIGHT_BLUE, COLOR_BLACK));
        screen_write("Executing iteration ");
        screen_write_dec(count);
        screen_write("\n");
        count++;

        // Busy wait
        for (volatile int i = 0; i < 10000000; i++);
    }

    screen_write_color("[Process B] ", MAKE_COLOR(COLOR_LIGHT_BLUE, COLOR_BLACK));
    screen_write("Completed!\n");

    // Process terminates
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

/**
 * Test process C - low priority background task
 */
void test_process_c(void) {
    int count = 0;
    while (count < 5) {
        screen_write_color("[Process C - Low Priority] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write("Iteration ");
        screen_write_dec(count);
        screen_write("\n");
        count++;

        // Busy wait
        for (volatile int i = 0; i < 10000000; i++);
    }

    screen_write_color("[Process C] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write("Done!\n");

    // Process terminates
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

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

    // Initialize memory management
    pmm_init();     // Physical memory manager
    heap_init();    // Kernel heap
    paging_init();  // Virtual memory (enables paging)

    // Initialize process management (multitasking)
    process_init();

    // Initialize timer (PIT)
    timer_init();

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
