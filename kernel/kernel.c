/**
 * Wittche Operating System - Main Kernel
 *
 * This is the main kernel entry point for the Wittche OS.
 * It initializes all system components and starts the shell.
 */

#include "../include/types.h"
#include "../include/screen.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/syscall.h"
#include "../include/pmm.h"
#include "../include/heap.h"
#include "../include/paging.h"
#include "../include/process.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/shell.h"
#include "../include/string.h"
#include "../include/userlib.h"
#include "../include/ramdisk.h"
#include "../include/fs.h"

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

    // Process exits cleanly
    process_exit();
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

    // Process exits cleanly
    process_exit();
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

    // Process exits cleanly
    process_exit();
}

/**
 * Test process - demonstrates sleep functionality
 */
void test_process_sleep(void) {
    int count = 0;
    while (count < 5) {
        screen_write_color("[Sleep Process] ", MAKE_COLOR(COLOR_LIGHT_MAGENTA, COLOR_BLACK));
        screen_write("Iteration ");
        screen_write_dec(count);
        screen_write(" - Going to sleep for 2000ms\n");

        // Sleep for 2 seconds (2000ms)
        process_sleep(2000);

        screen_write_color("[Sleep Process] ", MAKE_COLOR(COLOR_LIGHT_MAGENTA, COLOR_BLACK));
        screen_write("Woke up from sleep!\n");
        count++;
    }

    screen_write_color("[Sleep Process] ", MAKE_COLOR(COLOR_LIGHT_MAGENTA, COLOR_BLACK));
    screen_write("All iterations complete!\n");

    // Process exits cleanly
    process_exit();
}

/**
 * IPC Producer Process - sends messages to consumer
 */
void ipc_producer(void) {
    screen_write_color("[IPC Producer] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Starting message producer (PID ");
    screen_write_dec(process_current()->pid);
    screen_write(")\n");

    // Wait a bit for consumer to start
    process_sleep(500);

    // Find consumer process (PID should be current_pid + 1)
    pid_t consumer_pid = process_current()->pid + 1;

    for (int i = 0; i < 5; i++) {
        char message[64];
        strcpy(message, "Message #");
        char num_str[12];
        itoa(i, num_str, 10);
        strcat(message, num_str);
        strcat(message, " from Producer");

        screen_write_color("[IPC Producer] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
        screen_write("Sending: '");
        screen_write(message);
        screen_write("' to PID ");
        screen_write_dec(consumer_pid);
        screen_write("\n");

        int result = process_send_message(consumer_pid, message, strlen(message) + 1);
        if (result < 0) {
            screen_write_color("[IPC Producer] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
            screen_write("Failed to send message (error code: ");
            screen_write_dec(result);
            screen_write(")\n");
        }

        // Wait between messages
        process_sleep(1000);
    }

    screen_write_color("[IPC Producer] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("All messages sent!\n");

    // Process exits cleanly
    process_exit();
}

/**
 * IPC Consumer Process - receives messages from producer
 */
void ipc_consumer(void) {
    screen_write_color("[IPC Consumer] ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    screen_write("Starting message consumer (PID ");
    screen_write_dec(process_current()->pid);
    screen_write(")\n");

    char buffer[256];
    pid_t sender;
    int received_count = 0;

    while (received_count < 5) {
        // Check for messages
        if (process_has_messages() > 0) {
            int bytes = process_receive_message(buffer, sizeof(buffer), &sender);
            if (bytes > 0) {
                screen_write_color("[IPC Consumer] ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
                screen_write("Received from PID ");
                screen_write_dec(sender);
                screen_write(": '");
                screen_write(buffer);
                screen_write("'\n");
                received_count++;
            }
        } else {
            // No messages yet, yield CPU
            process_yield();
        }

        // Small delay
        process_sleep(100);
    }

    screen_write_color("[IPC Consumer] ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    screen_write("All messages received!\n");

    // Process exits cleanly
    process_exit();
}

/**
 * Syscall Test Process - demonstrates system call usage
 */
void test_syscall(void) {
    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Starting system call test\n");

    // Test SYS_GETPID
    int my_pid = getpid();
    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("My PID from syscall: ");
    screen_write_dec(my_pid);
    screen_write("\n");

    // Test SYS_WRITE
    const char *msg1 = "[Syscall Test] Testing SYS_WRITE syscall...\n";
    write(1, msg1, strlen(msg1));

    const char *msg2 = "[Syscall Test] Hello from userspace syscall!\n";
    write(1, msg2, strlen(msg2));

    // Test SYS_SLEEP
    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Sleeping for 1000ms using SYS_SLEEP...\n");
    sleep_ms(1000);

    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Woke up! Sleep syscall works.\n");

    // Test SYS_YIELD
    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Testing SYS_YIELD...\n");
    yield();

    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("Back from yield. All syscalls working!\n");

    // Test SYS_EXIT
    screen_write_color("[Syscall Test] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("All tests passed! Exiting with SYS_EXIT...\n");

    exit(0);  // This will terminate the process
}

/**
 * User Mode Test Process - runs in Ring 3
 * Simple test without string literals
 */
void test_usermode(void) {
    // Build messages on stack to avoid .rodata access
    char msg[64];

    // Test 1: Simple write
    msg[0] = '['; msg[1] = 'R'; msg[2] = 'i'; msg[3] = 'n'; msg[4] = 'g';
    msg[5] = ' '; msg[6] = '3'; msg[7] = ']'; msg[8] = ' ';
    msg[9] = 'H'; msg[10] = 'i'; msg[11] = '!'; msg[12] = '\n'; msg[13] = '\0';
    write(1, msg, 13);

    // Test 2: Get PID
    int my_pid = getpid();
    msg[0] = 'P'; msg[1] = 'I'; msg[2] = 'D'; msg[3] = '=';
    msg[4] = '0' + (my_pid % 10);
    msg[5] = '\n'; msg[6] = '\0';
    write(1, msg, 6);

    // Test 3: Sleep
    msg[0] = 'S'; msg[1] = 'l'; msg[2] = 'e'; msg[3] = 'e'; msg[4] = 'p';
    msg[5] = '.'; msg[6] = '.'; msg[7] = '\n'; msg[8] = '\0';
    write(1, msg, 8);
    sleep_ms(500);

    // Test 4: Woke up
    msg[0] = 'O'; msg[1] = 'K'; msg[2] = '!'; msg[3] = '\n'; msg[4] = '\0';
    write(1, msg, 4);

    // Exit
    exit(0);
}

/**
 * Main kernel entry point
 * Called from kernel_entry.asm after bootloader hands control
 */
void kernel_main(void) {
    // TEST: Write directly to VGA to see if we reach kernel_main
    volatile unsigned short *vga = (volatile unsigned short *)0xB8000;
    vga[0] = 0x0F4B; // 'K' - kernel_main reached

    // Initialize screen/VGA driver first
    screen_init();
    vga[1] = 0x0F53; // 'S' - screen_init done

    // Display boot messages
    screen_write_color("[BOOT] ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("Wittche OS kernel loaded at ");
    screen_write_hex(0x10000);
    screen_write("\n");
    vga[2] = 0x0F42; // 'B' - boot message done

    // Initialize Global Descriptor Table (GDT)
    gdt_init();
    vga[3] = 0x0F47; // 'G' - gdt_init done

    // Initialize Interrupt Descriptor Table
    idt_init();
    vga[4] = 0x0F49; // 'I' - idt_init done

    // Initialize system calls (INT 0x80)
    syscall_init();
    vga[5] = 0x0F59; // 'Y' - syscall_init done

    // Initialize memory management
    pmm_init();     // Physical memory manager
    vga[6] = 0x0F50; // 'P' - pmm_init done

    heap_init();    // Kernel heap
    vga[7] = 0x0F48; // 'H' - heap_init done

    paging_init();  // Virtual memory (enables paging)
    vga[8] = 0x0F56; // 'V' - paging_init done

    ramdisk_init(); // RAM disk (virtual disk in memory) - after paging!
    vga[9] = 0x0F52; // 'R' - ramdisk_init done

    // Initialize file system (format on first boot, then mount)
    screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("Checking file system status...\n");

    if (!fs_is_initialized()) {
        screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write("File system not initialized, formatting...\n");
        fs_format();  // Create new file system
        screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
        screen_write("Format complete, mounting file system...\n");
    } else {
        screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
        screen_write("File system already initialized, mounting...\n");
    }

    fs_init();  // Mount file system
    vga[10] = 0x0F46; // 'F' - fs_init done

    if (fs_is_initialized()) {
        screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
        screen_write("File system ready!\n");
    } else {
        screen_write_color("[KERNEL] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write("ERROR: File system mount failed!\n");
    }

    // Initialize process management (multitasking)
    process_init();
    vga[11] = 0x0F4F; // 'O' - process_init done

    // Initialize timer (PIT)
    timer_init();
    vga[12] = 0x0F54; // 'T' - timer_init done

    // Initialize keyboard driver
    keyboard_init();
    vga[13] = 0x0F4C; // 'L' - keyboard_init done

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
