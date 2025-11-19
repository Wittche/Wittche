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
 */
void test_usermode(void) {
    // This function runs in Ring 3 (user mode)!
    const char *msg = "[User Mode Test] Hello from Ring 3 (User Mode)!\n";
    write(1, msg, strlen(msg));

    // Test getpid syscall from user mode
    int my_pid = getpid();
    char msg2[64];
    strcpy(msg2, "[User Mode Test] My PID from user mode: ");
    char pid_str[12];
    itoa(my_pid, pid_str, 10);
    strcat(msg2, pid_str);
    strcat(msg2, "\n");
    write(1, msg2, strlen(msg2));

    // Test sleep syscall from user mode
    const char *msg3 = "[User Mode Test] Sleeping for 1000ms in user mode...\n";
    write(1, msg3, strlen(msg3));
    sleep_ms(1000);

    const char *msg4 = "[User Mode Test] Woke up! All syscalls working from Ring 3!\n";
    write(1, msg4, strlen(msg4));

    // Exit using syscall
    const char *msg5 = "[User Mode Test] Exiting user mode process...\n";
    write(1, msg5, strlen(msg5));

    exit(0);
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

    // Initialize Global Descriptor Table (GDT)
    gdt_init();

    // Initialize Interrupt Descriptor Table
    idt_init();

    // Initialize system calls (INT 0x80)
    syscall_init();

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
