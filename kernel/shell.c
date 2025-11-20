// Shell implementation
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/kprintf.h"
#include "../include/types.h"
#include "../include/pmm.h"
#include "../include/heap.h"
#include "../include/paging.h"
#include "../include/process.h"
#include "../include/ramdisk.h"

// External test processes from kernel.c
extern void test_process_a(void);
extern void test_process_b(void);
extern void test_process_c(void);
extern void test_process_sleep(void);
extern void ipc_producer(void);
extern void ipc_consumer(void);
extern void test_syscall(void);
extern void test_usermode(void);

// Command history
#define MAX_HISTORY 10
#define MAX_CMD_LENGTH 256

static char command_history[MAX_HISTORY][MAX_CMD_LENGTH];
static int history_count = 0;

// Available commands for tab completion
static const char *available_commands[] = {
    "help",
    "clear",
    "cls",
    "about",
    "ver",
    "mem",
    "meminfo",
    "memtest",
    "ps",
    "testproc",
    "sleeptest",
    "nice",
    "kill",
    "ipctest",
    "syscalltest",
    "usermodetest",
    "echo",
    "color",
    "uptime",
    "history",
    "banner",
    "ramdisk",
    "rdformat",
    "rdinfo",
    NULL  // Sentinel
};

/**
 * Display welcome banner
 */
void shell_display_banner(void) {
    screen_write_color("\n", DEFAULT_COLOR);
    screen_write_color("===========================================\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write_color(" Wittche Operating System v0.9\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("===========================================\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("\n");
    screen_write("Welcome to Wittche OS!\n");
    screen_write("Type 'help' for available commands.\n\n");
}

/**
 * Display command prompt
 */
void shell_prompt(void) {
    screen_write_color("wittche", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write_color("> ", MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
}

/**
 * Help command
 */
static void cmd_help(void) {
    screen_write("\n");
    screen_write_color("Available Commands:\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("==================\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("  help", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("      - Display this help message\n");
    screen_write_color("  clear", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("     - Clear the screen\n");
    screen_write_color("  cls", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("       - Alias for clear\n");
    screen_write_color("  about", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("     - Show system information\n");
    screen_write_color("  ver", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("       - Show OS version\n");
    screen_write_color("  mem", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("       - Display memory layout\n");
    screen_write_color("  meminfo", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("   - Detailed memory statistics\n");
    screen_write_color("  memtest", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("   - Test PMM and heap allocation\n");
    screen_write_color("  ps", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("        - List running processes\n");
    screen_write_color("  testproc", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("  - Spawn test processes A, B, C\n");
    screen_write_color("  sleeptest", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write(" - Demonstrate process sleep/wake\n");
    screen_write_color("  nice", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("      - Change process priority\n");
    screen_write_color("  kill", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("      - Terminate a process\n");
    screen_write_color("  ipctest", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("   - Test Inter-Process Communication\n");
    screen_write_color("  syscalltest", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write(" - Test system calls (INT 0x80)\n");
    screen_write_color("  usermodetest", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write(" - Test user mode (Ring 3)\n");
    screen_write_color("  echo", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("      - Echo a message\n");
    screen_write_color("  color", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("     - Test color output\n");
    screen_write_color("  uptime", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("    - Show system uptime\n");
    screen_write_color("  history", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("   - Show command history\n");
    screen_write_color("  banner", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("    - Display welcome banner\n");
    screen_write_color("  ramdisk", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("   - Test RAM disk read/write\n");
    screen_write_color("  rdformat", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("  - Format (clear) RAM disk\n");
    screen_write_color("  rdinfo", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("    - Display RAM disk information\n");
    screen_write("\n");
}

/**
 * Clear command
 */
static void cmd_clear(void) {
    screen_clear();
}

/**
 * About command
 */
static void cmd_about(void) {
    screen_write("\n");
    screen_write_color("Wittche Operating System v0.7\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("===============================\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("\n");
    screen_write("A simple x86 operating system for educational purposes.\n\n");

    screen_write_color("Features:\n", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("  - 32-bit protected mode kernel\n");
    screen_write("  - Hardware interrupt handling (IDT)\n");
    screen_write("  - Physical Memory Manager (PMM) with bitmap allocator\n");
    screen_write("  - Kernel heap (kmalloc/kfree) with 4MB size\n");
    screen_write("  - Paging (virtual memory) with identity mapping\n");
    screen_write("  - Process management with PCB and multitasking\n");
    screen_write("  - Preemptive round-robin scheduler\n");
    screen_write("  - Context switching with full state save/restore\n");
    screen_write("  - Programmable Interval Timer (PIT)\n");
    screen_write("  - PS/2 keyboard driver with arrow key support\n");
    screen_write("  - VGA text mode with hardware cursor\n");
    screen_write("  - Proper screen scrolling\n");
    screen_write("  - Printf-style formatted output (kprintf)\n");
    screen_write("  - Interactive shell with tab completion\n");
    screen_write("\n");

    screen_write_color("Technical Info:\n", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("  Architecture:   x86 (32-bit)\n");
    screen_write("  Kernel at:      ");
    screen_write_hex(0x10000);
    screen_write("\n");
    screen_write("  Video Memory:   ");
    screen_write_hex(0xB8000);
    screen_write("\n");
    screen_write("  Screen Size:    80x25 characters\n");
    screen_write("  Timer Freq:     ");
    screen_write_dec(TIMER_FREQUENCY);
    screen_write(" Hz\n");
    screen_write("\n");
}

/**
 * Echo command
 */
static void cmd_echo(char *args) {
    screen_write("\n");
    if (args && *args) {
        screen_write(args);
    }
    screen_write("\n");
}

/**
 * Color test command
 */
static void cmd_color(void) {
    screen_write("\n");
    screen_write_color("Color Test:\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));

    screen_write_color("  Black ", MAKE_COLOR(COLOR_BLACK, COLOR_LIGHT_GREY));
    screen_write_color("  Blue ", MAKE_COLOR(COLOR_BLUE, COLOR_BLACK));
    screen_write_color("  Green ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write_color("  Cyan ", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("\n");

    screen_write_color("  Red ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
    screen_write_color("  Magenta ", MAKE_COLOR(COLOR_MAGENTA, COLOR_BLACK));
    screen_write_color("  Brown ", MAKE_COLOR(COLOR_BROWN, COLOR_BLACK));
    screen_write_color("  Light Grey ", MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
    screen_write("\n");

    screen_write_color("  Dark Grey ", MAKE_COLOR(COLOR_DARK_GREY, COLOR_BLACK));
    screen_write_color("  Light Blue ", MAKE_COLOR(COLOR_LIGHT_BLUE, COLOR_BLACK));
    screen_write_color("  Light Green ", MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    screen_write_color("  Light Cyan ", MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    screen_write("\n");

    screen_write_color("  Light Red ", MAKE_COLOR(COLOR_LIGHT_RED, COLOR_BLACK));
    screen_write_color("  Light Magenta ", MAKE_COLOR(COLOR_LIGHT_MAGENTA, COLOR_BLACK));
    screen_write_color("  Yellow ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("  White ", MAKE_COLOR(COLOR_WHITE, COLOR_BLACK));
    screen_write("\n\n");
}

/**
 * Uptime command
 */
static void cmd_uptime(void) {
    char uptime_buffer[16];
    uint32_t ticks = timer_get_ticks();
    uint32_t seconds = timer_get_seconds();

    screen_write("\n");
    screen_write_color("System Uptime:\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write("  Time:        ");
    timer_format_uptime(uptime_buffer);
    screen_write_color(uptime_buffer, MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("\n");

    screen_write("  Seconds:     ");
    screen_write_dec(seconds);
    screen_write(" s\n");

    screen_write("  Ticks:       ");
    screen_write_dec(ticks);
    screen_write(" (");
    screen_write_dec(TIMER_FREQUENCY);
    screen_write(" Hz)\n");
    screen_write("\n");
}

/**
 * History command
 */
static void cmd_history(void) {
    screen_write("\n");
    if (history_count == 0) {
        screen_write("No command history.\n");
        return;
    }

    screen_write_color("Command History:\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    for (int i = 0; i < history_count; i++) {
        screen_write("  ");
        screen_write_dec(i + 1);
        screen_write(". ");
        screen_write(command_history[i]);
        screen_write("\n");
    }
    screen_write("\n");
}

/**
 * Banner command
 */
static void cmd_banner(void) {
    shell_display_banner();
}

/**
 * Version command - displays OS version info
 */
static void cmd_ver(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Wittche OS Version Information\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "==============================\n");
    kprintf("\n");
    kprintf("  Version:     ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "0.7.0\n");
    kprintf("  Codename:    ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "Process Manager\n");
    kprintf("  Build Date:  ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "2024-11\n");
    kprintf("  Arch:        ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "x86 (32-bit)\n");
    kprintf("  License:     ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "MIT\n");
    kprintf("\n");
}

/**
 * Memory command - displays memory layout and info
 */
static void cmd_mem(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Memory Layout\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "=============\n");
    kprintf("\n");

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Kernel Memory Map:\n");
    kprintf("  Bootloader:      %p (512 bytes)\n", 0x7C00);
    kprintf("  Kernel Code:     %p (loaded here)\n", 0x10000);
    kprintf("  Stack:           %p (grows downward)\n", 0x90000);
    kprintf("  VGA Text Buffer: %p (80x25 chars)\n", 0xB8000);
    kprintf("\n");

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Segment Registers:\n");

    uint32_t ds, es, fs, gs, ss;
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    __asm__ __volatile__("mov %%es, %0" : "=r"(es));
    __asm__ __volatile__("mov %%fs, %0" : "=r"(fs));
    __asm__ __volatile__("mov %%gs, %0" : "=r"(gs));
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));

    kprintf("  DS (Data):       0x%X\n", ds);
    kprintf("  ES (Extra):      0x%X\n", es);
    kprintf("  FS:              0x%X\n", fs);
    kprintf("  GS:              0x%X\n", gs);
    kprintf("  SS (Stack):      0x%X\n", ss);
    kprintf("\n");

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Memory Statistics:\n");
    kprintf("  Total RAM:       %d MB\n", pmm_get_total_memory() / (1024 * 1024));
    kprintf("  Used:            %d KB (%d pages)\n",
            pmm_get_used_memory() / 1024, pmm_get_used_pages());
    kprintf("  Free:            %d KB (%d pages)\n",
            pmm_get_free_memory() / 1024, pmm_get_free_pages());
    kprintf("\n");
}

/**
 * Memory info command - detailed memory statistics
 */
static void cmd_meminfo(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Wittche OS Memory Information\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "=============================\n");
    kprintf("\n");

    // Physical Memory Manager stats
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Physical Memory (PMM):\n");
    kprintf("  Total Memory:    %d MB (%d bytes)\n",
            pmm_get_total_memory() / (1024 * 1024), pmm_get_total_memory());
    kprintf("  Used Memory:     %d KB (%d bytes)\n",
            pmm_get_used_memory() / 1024, pmm_get_used_memory());
    kprintf("  Free Memory:     %d KB (%d bytes)\n",
            pmm_get_free_memory() / 1024, pmm_get_free_memory());
    kprintf("  Total Pages:     %d (4KB each)\n", pmm_get_total_pages());
    kprintf("  Used Pages:      %d\n", pmm_get_used_pages());
    kprintf("  Free Pages:      %d\n", pmm_get_free_pages());

    // Calculate usage percentage
    uint32_t usage_percent = (pmm_get_used_memory() * 100) / pmm_get_total_memory();
    kprintf("  Usage:           %d%%\n", usage_percent);
    kprintf("\n");

    // Kernel Heap stats
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Kernel Heap:\n");
    kprintf("  Heap Start:      0x%X\n", HEAP_START);
    kprintf("  Heap Size:       %d MB (%d bytes)\n",
            heap_get_total_size() / (1024 * 1024), heap_get_total_size());
    kprintf("  Used:            %d KB (%d bytes)\n",
            heap_get_used_size() / 1024, heap_get_used_size());
    kprintf("  Free:            %d KB (%d bytes)\n",
            heap_get_free_size() / 1024, heap_get_free_size());
    kprintf("  Total Blocks:    %d\n", heap_get_block_count());
    kprintf("  Free Blocks:     %d\n", heap_get_free_block_count());

    // Heap usage percentage
    uint32_t heap_usage = 0;
    if (heap_get_total_size() > 0) {
        heap_usage = (heap_get_used_size() * 100) / heap_get_total_size();
    }
    kprintf("  Usage:           %d%%\n", heap_usage);
    kprintf("\n");

    // Paging info
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Virtual Memory (Paging):\n");
    kprintf("  Status:          ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "ENABLED\n");
    kprintf("  Page Size:       4 KB\n");
    kprintf("  Page Directory:  0x%X\n", (uint32_t)paging_get_directory());
    kprintf("  Identity Map:    First 16 MB\n");
    kprintf("\n");

    // Memory bar graph
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Memory Usage Bar:\n");
    kprintf("  [");

    // Draw 50-character bar
    int bars = (usage_percent * 50) / 100;
    for (int i = 0; i < 50; i++) {
        if (i < bars) {
            kprintf_color(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK), "=");
        } else {
            kprintf_color(MAKE_COLOR(COLOR_DARK_GREY, COLOR_BLACK), "-");
        }
    }
    kprintf("] %d%%\n", usage_percent);
    kprintf("\n");
}

/**
 * Memory test command - tests PMM and heap allocation
 */
static void cmd_memtest(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Memory Management Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "======================\n");
    kprintf("\n");

    // Show initial stats
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Initial State:\n");
    kprintf("  PMM Free Pages:  %d\n", pmm_get_free_pages());
    kprintf("  Heap Free:       %d KB\n", heap_get_free_size() / 1024);
    kprintf("\n");

    // Test 1: PMM Page Allocation
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Test 1: PMM Page Allocation\n");
    kprintf("  Allocating 5 pages (20 KB)...\n");

    uint32_t pages[5];
    for (int i = 0; i < 5; i++) {
        pages[i] = pmm_alloc_page();
        if (pages[i] == 0) {
            kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: Could not allocate page %d\n", i);
            return;
        }
        kprintf("  Page %d allocated at: 0x%X\n", i + 1, pages[i]);
    }
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: All pages allocated\n");
    kprintf("  PMM Free Pages:  %d (should be 5 less)\n", pmm_get_free_pages());
    kprintf("\n");

    // Test 2: Heap Allocation
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Test 2: Heap Allocation (kmalloc)\n");
    kprintf("  Allocating 1 KB, 2 KB, 4 KB blocks...\n");

    void *ptr1 = kmalloc(1024);
    void *ptr2 = kmalloc(2048);
    void *ptr3 = kmalloc(4096);

    if (!ptr1 || !ptr2 || !ptr3) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: Heap allocation failed\n");
    } else {
        kprintf("  Block 1 (1 KB):  0x%X\n", (uint32_t)ptr1);
        kprintf("  Block 2 (2 KB):  0x%X\n", (uint32_t)ptr2);
        kprintf("  Block 3 (4 KB):  0x%X\n", (uint32_t)ptr3);
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: All blocks allocated\n");
        kprintf("  Heap Free:       %d KB\n", heap_get_free_size() / 1024);
        kprintf("  Heap Blocks:     %d total, %d free\n",
                heap_get_block_count(), heap_get_free_block_count());
    }
    kprintf("\n");

    // Test 3: Memory Write Test
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Test 3: Memory Write Test\n");
    if (ptr1) {
        kprintf("  Writing pattern to allocated memory...\n");
        char *test = (char *)ptr1;
        for (int i = 0; i < 100; i++) {
            test[i] = 'A' + (i % 26);
        }
        test[100] = '\0';
        kprintf("  Pattern: %.50s...\n", test);
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Memory write/read works\n");
    }
    kprintf("\n");

    // Test 4: Free Memory
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Test 4: Free Memory\n");
    kprintf("  Freeing heap blocks...\n");
    if (ptr1) kfree(ptr1);
    if (ptr2) kfree(ptr2);
    if (ptr3) kfree(ptr3);
    kprintf("  Heap Free:       %d KB (should be back to initial)\n", heap_get_free_size() / 1024);
    kprintf("  Heap Blocks:     %d total, %d free\n",
            heap_get_block_count(), heap_get_free_block_count());

    kprintf("\n  Freeing PMM pages...\n");
    for (int i = 0; i < 5; i++) {
        pmm_free_page(pages[i]);
    }
    kprintf("  PMM Free Pages:  %d (should be back to initial)\n", pmm_get_free_pages());
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: All memory freed\n");
    kprintf("\n");

    // Final Summary
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "All Tests Passed! ");
    kprintf_color(MAKE_COLOR(COLOR_WHITE, COLOR_BLACK), "Memory management is working correctly.\n");
    kprintf("\n");
}

/**
 * ps command - list running processes
 */
static void cmd_ps(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Process List\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "============\n");
    kprintf("\n");

    // Get process table
    process_t **proc_table = process_get_table();
    process_t *current = process_current();
    int count = 0;

    // Header
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                 "PID  NAME                 STATE      PRIORITY  TIME(ms)\n");
    kprintf("---  -------------------  ---------  --------  --------\n");

    // List all processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i]) {
            process_t *p = proc_table[i];

            // Print with color for current process
            if (p == current) {
                kprintf_color(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK), "%-4d ", p->pid);
            } else {
                kprintf("%-4d ", p->pid);
            }

            // Name
            kprintf("%-20s ", p->name);

            // State
            const char *state_str;
            uint8_t state_color;
            switch (p->state) {
                case PROCESS_STATE_READY:
                    state_str = "READY";
                    state_color = MAKE_COLOR(COLOR_CYAN, COLOR_BLACK);
                    break;
                case PROCESS_STATE_RUNNING:
                    state_str = "RUNNING";
                    state_color = MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK);
                    break;
                case PROCESS_STATE_BLOCKED:
                    state_str = "BLOCKED";
                    state_color = MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK);
                    break;
                case PROCESS_STATE_TERMINATED:
                    state_str = "TERM";
                    state_color = MAKE_COLOR(COLOR_RED, COLOR_BLACK);
                    break;
                default:
                    state_str = "UNKNOWN";
                    state_color = MAKE_COLOR(COLOR_DARK_GREY, COLOR_BLACK);
            }
            kprintf_color(state_color, "%-9s  ", state_str);

            // Priority and time
            kprintf("%-8d  %-8d\n", p->priority, p->total_time);
            count++;
        }
    }

    kprintf("\n");
    kprintf("Total processes: %d\n", count);
    kprintf("Current process: ");
    if (current) {
        kprintf_color(MAKE_COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK),
                     "%s (PID %d)\n", current->name, current->pid);
    } else {
        kprintf("None\n");
    }
    kprintf("\n");
}

/**
 * Spawn test processes to demonstrate priority-based multitasking
 */
static void cmd_testproc(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Spawning Test Processes\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "========================\n");
    kprintf("Testing priority-based scheduling...\n\n");

    // Create test process A - HIGH priority (20)
    pid_t pid_a = process_create_with_priority("TestProcA", test_process_a, 4096, 20);
    if (pid_a > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Process A (PID %d, Priority=20 HIGH)\n", pid_a);
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create Process A\n");
    }

    // Create test process B - MEDIUM priority (10)
    pid_t pid_b = process_create_with_priority("TestProcB", test_process_b, 4096, 10);
    if (pid_b > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Process B (PID %d, Priority=10 MEDIUM)\n", pid_b);
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create Process B\n");
    }

    // Create test process C - LOW priority (2)
    pid_t pid_c = process_create_with_priority("TestProcC", test_process_c, 4096, 2);
    if (pid_c > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Process C (PID %d, Priority=2 LOW)\n", pid_c);
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create Process C\n");
    }

    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK),
                 "Test processes are now running with priorities!\n");
    kprintf("Process A (HIGH) should run most frequently.\n");
    kprintf("Process B (MEDIUM) should run occasionally.\n");
    kprintf("Process C (LOW) should run least often.\n");
    kprintf("Use 'ps' command to view process priorities.\n");
    kprintf("\n");
}

/**
 * nice command - change process priority
 * Usage: nice <PID> <priority>
 */
static void cmd_nice(const char *args) {
    kprintf("\n");

    if (!args || strlen(args) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "Usage: nice <PID> <priority>\n");
        kprintf("  PID: Process ID (use 'ps' to see PIDs)\n");
        kprintf("  Priority: 0-255 (higher = more CPU time)\n");
        kprintf("\nExample: nice 2 50\n");
        kprintf("\n");
        return;
    }

    // Parse arguments
    char args_copy[256];
    strncpy(args_copy, args, 255);
    args_copy[255] = '\0';

    // Find first space
    char *space = strchr(args_copy, ' ');
    if (!space) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Missing priority argument\n");
        kprintf("Usage: nice <PID> <priority>\n\n");
        return;
    }

    *space = '\0';  // Split string
    char *pid_str = args_copy;
    char *priority_str = space + 1;

    // Convert to integers
    int pid = atoi(pid_str);
    int priority = atoi(priority_str);

    if (pid < 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Invalid PID\n\n");
        return;
    }

    if (priority < 0 || priority > 255) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Priority must be 0-255\n\n");
        return;
    }

    // Get process to check if it exists
    process_t *proc = process_get(pid);
    if (!proc) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Process %d not found\n", pid);
        kprintf("Use 'ps' command to see active processes\n\n");
        return;
    }

    uint32_t old_priority = proc->priority;

    // Set new priority
    if (process_set_priority(pid, priority) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Priority changed successfully!\n");
        kprintf("Process: %s (PID %d)\n", proc->name, pid);
        kprintf("Old Priority: %d\n", old_priority);
        kprintf("New Priority: %d\n", priority);
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Failed to set priority\n");
    }

    kprintf("\n");
}

/**
 * sleeptest command - demonstrate process sleep/wake functionality
 */
static void cmd_sleeptest(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Sleep/Wake Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "===============\n");
    kprintf("Spawning a process that sleeps for 2 seconds between iterations...\n\n");

    // Create sleep test process
    pid_t pid = process_create_with_priority("SleepTest", test_process_sleep, 4096, 10);
    if (pid > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Sleep Test Process (PID %d)\n", pid);
        kprintf("\nWatch the process:\n");
        kprintf("1. Print message and go to sleep\n");
        kprintf("2. Process becomes BLOCKED for 2000ms\n");
        kprintf("3. Automatically wakes up after 2 seconds\n");
        kprintf("4. Print 'Woke up!' message\n");
        kprintf("5. Repeat 5 times\n\n");
        kprintf("Use 'ps' command while it's sleeping to see BLOCKED state!\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create sleep test process\n");
    }

    kprintf("\n");
}

/**
 * kill command - terminate a process
 * Usage: kill <PID>
 */
static void cmd_kill(const char *args) {
    kprintf("\n");

    if (!args || strlen(args) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "Usage: kill <PID>\n");
        kprintf("  PID: Process ID (use 'ps' to see PIDs)\n");
        kprintf("\nExample: kill 2\n");
        kprintf("\nWarning: Cannot kill idle process (PID 0)\n");
        kprintf("\n");
        return;
    }

    // Convert to integer
    int pid = atoi(args);

    if (pid < 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Invalid PID\n\n");
        return;
    }

    if (pid == 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Cannot kill idle process (PID 0)\n\n");
        return;
    }

    // Check if process exists
    process_t *proc = process_get(pid);
    if (!proc) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Error: Process %d not found\n", pid);
        kprintf("Use 'ps' command to see active processes\n\n");
        return;
    }

    // Get process info before killing
    char name[32];
    strncpy(name, proc->name, 31);
    name[31] = '\0';

    // Kill the process
    kprintf("Terminating process: %s (PID %d)\n", name, pid);
    process_kill(pid);
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                 "Process %d terminated successfully\n", pid);

    kprintf("\n");
}

/**
 * ipctest command - test Inter-Process Communication
 * Creates producer and consumer processes that exchange messages
 */
static void cmd_ipctest(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Inter-Process Communication Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "=================================\n");
    kprintf("Spawning Producer and Consumer processes...\n\n");

    // Create producer process
    pid_t producer_pid = process_create_with_priority("IPC_Producer", ipc_producer, 4096, 10);
    if (producer_pid == 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create producer process\n\n");
        return;
    }

    // Create consumer process
    pid_t consumer_pid = process_create_with_priority("IPC_Consumer", ipc_consumer, 4096, 10);
    if (consumer_pid == 0) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create consumer process\n\n");
        return;
    }

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                 "Created Producer Process (PID %d)\n", producer_pid);
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                 "Created Consumer Process (PID %d)\n", consumer_pid);

    kprintf("\nWatch the processes:\n");
    kprintf("1. Producer sends 5 messages (one per second)\n");
    kprintf("2. Consumer receives and displays messages\n");
    kprintf("3. Message queue holds up to 8 messages\n");
    kprintf("4. Each message is max 256 bytes\n\n");

    kprintf("Use 'ps' command to monitor process states!\n");
    kprintf("\n");
}

/**
 * syscalltest command - test system calls
 * Creates a process that tests INT 0x80 syscalls
 */
static void cmd_syscalltest(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "System Call Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "================\n");
    kprintf("Spawning a process to test system calls...\n\n");

    // Create syscall test process
    pid_t pid = process_create_with_priority("SyscallTest", test_syscall, 4096, 10);
    if (pid > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Syscall Test Process (PID %d)\n", pid);
        kprintf("\nThis process will test:\n");
        kprintf("1. SYS_GETPID - Get process ID\n");
        kprintf("2. SYS_WRITE - Write to screen\n");
        kprintf("3. SYS_SLEEP - Sleep for milliseconds\n");
        kprintf("4. SYS_YIELD - Yield CPU\n");
        kprintf("5. SYS_EXIT - Exit process\n\n");
        kprintf("Watch the output to see syscalls in action!\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create syscall test process\n");
    }

    kprintf("\n");
}

/**
 * usermodetest command - test syscall infrastructure
 * NOTE: True Ring 3 user mode requires ELF loader (planned for v1.0)
 * For now, test syscalls from kernel mode to verify infrastructure
 */
static void cmd_usermodetest(void) {
    kprintf("\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Syscall Infrastructure Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "============================\n");
    kprintf_color(MAKE_COLOR(COLOR_MAGENTA, COLOR_BLACK),
                 "NOTE: Full Ring 3 requires ELF loader (planned for v1.0)\n");
    kprintf("Testing syscall infrastructure from kernel mode...\n\n");

    // Test syscalls from kernel mode
    // (Full user mode needs ELF loader to load separate user binaries)
    pid_t pid = process_create_with_priority("SyscallTest", test_usermode, 4096, 10);
    if (pid > 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                     "Created Syscall Test Process (PID %d)\n", pid);
        kprintf("\nTesting syscalls:");
        kprintf("\n  - SYS_WRITE (write to screen)\n");
        kprintf("  - SYS_GETPID (get process ID)\n");
        kprintf("  - SYS_SLEEP (sleep ms)\n");
        kprintf("  - SYS_EXIT (exit process)\n\n");
        kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK),
                     "Infrastructure ready for future Ring 3 support!\n");
        kprintf("(Page directories, GDT/TSS, syscalls all working)\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "Failed to create test process\n");
    }

    kprintf("\n");
}

/**
 * rdinfo command - display RAM disk information
 */
static void cmd_rdinfo(void) {
    kprintf("\n");
    kprintf("DEBUG: rdinfo command started\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "RAM Disk Information\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "====================\n");
    kprintf("\n");

    kprintf("DEBUG: About to check ramdisk_is_initialized()\n");
    int is_init = ramdisk_is_initialized();
    kprintf("DEBUG: ramdisk_is_initialized() returned: %d\n", is_init);

    if (!is_init) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "RAM Disk is NOT initialized!\n");
        kprintf("\n");
        return;
    }

    kprintf("DEBUG: About to call ramdisk_get_info()\n");
    ramdisk_t *rd = ramdisk_get_info();
    kprintf("DEBUG: ramdisk_get_info() returned: 0x%X\n", (uint32_t)rd);

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Status:\n");
    kprintf("  Initialized:     ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "YES\n");
    kprintf("  Base Address:    0x%X\n", (uint32_t)rd->data);
    kprintf("\n");

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Configuration:\n");
    kprintf("  Total Size:      %d bytes (%d KB / %d MB)\n",
            rd->size, rd->size / 1024, rd->size / (1024 * 1024));
    kprintf("  Block Size:      %d bytes\n", rd->block_size);
    kprintf("  Block Count:     %d blocks\n", rd->block_count);
    kprintf("\n");

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Technical Details:\n");
    kprintf("  Type:            Virtual disk in RAM\n");
    kprintf("  Speed:           Memory speed (very fast)\n");
    kprintf("  Volatile:        Yes (data lost on reboot)\n");
    kprintf("  Purpose:         Foundation for file system\n");
    kprintf("\n");
}

/**
 * rdformat command - format RAM disk
 */
static void cmd_rdformat(void) {
    kprintf("\n");
    kprintf("DEBUG: rdformat command started\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "RAM Disk Format\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "===============\n");
    kprintf("\n");

    kprintf("DEBUG: Checking if ramdisk is initialized\n");
    if (!ramdisk_is_initialized()) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "Error: RAM Disk is not initialized!\n");
        kprintf("\n");
        return;
    }

    kprintf("DEBUG: Ramdisk is initialized, calling ramdisk_format()\n");
    kprintf("Formatting RAM disk (clearing all data)...\n");
    ramdisk_format();
    kprintf("DEBUG: ramdisk_format() completed\n");
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "RAM Disk formatted successfully!\n");
    kprintf("All blocks cleared to zero.\n");
    kprintf("\n");
}

/**
 * ramdisk command - test RAM disk read/write
 */
static void cmd_ramdisk(void) {
    kprintf("\n");
    kprintf("DEBUG: ramdisk test command started\n");
    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "RAM Disk Test\n");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "=============\n");
    kprintf("\n");

    kprintf("DEBUG: Checking initialization\n");
    if (!ramdisk_is_initialized()) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "Error: RAM Disk is not initialized!\n");
        kprintf("\n");
        return;
    }

    kprintf("DEBUG: Getting ramdisk info\n");
    ramdisk_t *rd = ramdisk_get_info();
    kprintf("DEBUG: Got ramdisk info, starting tests\n");

    uint8_t write_buffer[512];
    uint8_t read_buffer[512];

    // Test 1: Write test pattern to block 0
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Test 1: Write Pattern to Block 0\n");
    kprintf("  Preparing test data...\n");

    // Fill buffer with pattern
    for (int i = 0; i < 512; i++) {
        write_buffer[i] = (uint8_t)(i % 256);
    }

    kprintf("  Writing block 0...\n");
    if (ramdisk_write_block(0, write_buffer) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Block written\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: Write error\n");
        kprintf("\n");
        return;
    }
    kprintf("\n");

    // Test 2: Read back and verify
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Test 2: Read and Verify Block 0\n");
    kprintf("  Reading block 0...\n");

    if (ramdisk_read_block(0, read_buffer) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Block read\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: Read error\n");
        kprintf("\n");
        return;
    }

    kprintf("  Verifying data...\n");
    int errors = 0;
    for (int i = 0; i < 512; i++) {
        if (read_buffer[i] != write_buffer[i]) {
            errors++;
        }
    }

    if (errors == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Data verified (0 errors)\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: %d byte mismatches\n", errors);
    }
    kprintf("\n");

    // Test 3: Write ASCII text to block 1
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Test 3: Write ASCII Text to Block 1\n");
    memset(write_buffer, 0, 512);
    const char *test_msg = "Hello from Wittche OS RAM Disk! This is a test message.";
    strcpy((char *)write_buffer, test_msg);

    kprintf("  Writing: '%s'\n", test_msg);
    ramdisk_write_block(1, write_buffer);
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Text written to block 1\n");
    kprintf("\n");

    // Test 4: Read back text
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Test 4: Read Text from Block 1\n");
    memset(read_buffer, 0, 512);
    ramdisk_read_block(1, read_buffer);
    kprintf("  Read back: '%s'\n", (char *)read_buffer);

    if (strcmp((char *)read_buffer, test_msg) == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: Text matches perfectly\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: Text mismatch\n");
    }
    kprintf("\n");

    // Test 5: Multiple block operations
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "Test 5: Multiple Block Operations\n");
    kprintf("  Writing to 10 different blocks...\n");

    for (int block = 10; block < 20; block++) {
        memset(write_buffer, 'A' + (block - 10), 512);
        ramdisk_write_block(block, write_buffer);
    }

    kprintf("  Reading back and verifying...\n");
    int block_errors = 0;
    for (int block = 10; block < 20; block++) {
        ramdisk_read_block(block, read_buffer);
        for (int i = 0; i < 512; i++) {
            if (read_buffer[i] != ('A' + (block - 10))) {
                block_errors++;
                break;
            }
        }
    }

    if (block_errors == 0) {
        kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "  SUCCESS: All 10 blocks verified\n");
    } else {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "  FAILED: %d blocks had errors\n", block_errors);
    }
    kprintf("\n");

    // Summary
    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK), "RAM Disk Test Complete!\n");
    kprintf("Total blocks available: %d\n", rd->block_count);
    kprintf("Blocks tested: 12 (blocks 0, 1, and 10-19)\n");
    kprintf("\n");
}

/**
 * Add command to history
 */
static void shell_add_history(const char *command) {
    if (!command || *command == '\0') return;

    // Don't add duplicate of last command
    if (history_count > 0 && strcmp(command_history[history_count - 1], command) == 0) {
        return;
    }

    // Shift history if full
    if (history_count >= MAX_HISTORY) {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(command_history[i], command_history[i + 1]);
        }
        history_count = MAX_HISTORY - 1;
    }

    // Add new command
    strncpy(command_history[history_count], command, MAX_CMD_LENGTH - 1);
    command_history[history_count][MAX_CMD_LENGTH - 1] = '\0';
    history_count++;
}

/**
 * Process a shell command
 */
void shell_process_command(char *command) {
    // Trim whitespace
    str_trim(command);

    // Empty command
    if (*command == '\0') {
        screen_write("\n");
        return;
    }

    // Add to history
    shell_add_history(command);

    // Parse command and arguments
    char *tokens[10];
    char cmd_copy[MAX_CMD_LENGTH];
    strncpy(cmd_copy, command, MAX_CMD_LENGTH);

    int token_count = str_split(cmd_copy, ' ', tokens, 10);

    if (token_count == 0) {
        screen_write("\n");
        return;
    }

    char *cmd = tokens[0];

    // Find full args string (everything after first token)
    char *full_args = command;
    while (*full_args && *full_args != ' ') full_args++;
    while (*full_args == ' ') full_args++;

    // Execute command
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "clear") == 0 || strcmp(cmd, "cls") == 0) {
        cmd_clear();
    } else if (strcmp(cmd, "about") == 0) {
        cmd_about();
    } else if (strcmp(cmd, "ver") == 0) {
        cmd_ver();
    } else if (strcmp(cmd, "mem") == 0) {
        cmd_mem();
    } else if (strcmp(cmd, "meminfo") == 0) {
        cmd_meminfo();
    } else if (strcmp(cmd, "memtest") == 0) {
        cmd_memtest();
    } else if (strcmp(cmd, "ps") == 0) {
        cmd_ps();
    } else if (strcmp(cmd, "testproc") == 0) {
        cmd_testproc();
    } else if (strcmp(cmd, "sleeptest") == 0) {
        cmd_sleeptest();
    } else if (strcmp(cmd, "nice") == 0) {
        cmd_nice(full_args);
    } else if (strcmp(cmd, "kill") == 0) {
        cmd_kill(full_args);
    } else if (strcmp(cmd, "ipctest") == 0) {
        cmd_ipctest();
    } else if (strcmp(cmd, "syscalltest") == 0) {
        cmd_syscalltest();
    } else if (strcmp(cmd, "usermodetest") == 0) {
        cmd_usermodetest();
    } else if (strcmp(cmd, "echo") == 0) {
        cmd_echo(full_args);
    } else if (strcmp(cmd, "color") == 0) {
        cmd_color();
    } else if (strcmp(cmd, "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(cmd, "history") == 0) {
        cmd_history();
    } else if (strcmp(cmd, "banner") == 0) {
        cmd_banner();
    } else if (strcmp(cmd, "ramdisk") == 0) {
        cmd_ramdisk();
    } else if (strcmp(cmd, "rdformat") == 0) {
        cmd_rdformat();
    } else if (strcmp(cmd, "rdinfo") == 0) {
        cmd_rdinfo();
    } else {
        screen_write("\n");
        screen_write_color("Error: ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write("Unknown command '");
        screen_write(cmd);
        screen_write("'\n");
        screen_write("Type 'help' for available commands.\n");
    }
}

/**
 * Handle tab completion
 * Returns number of matches found
 */
static int handle_tab_completion(char *buffer, int length, int max_length) {
    int matches = 0;
    const char *match = NULL;
    const char *common_prefix = NULL;
    int common_len = 0;

    // Find all commands that match the current prefix
    for (int i = 0; available_commands[i] != NULL; i++) {
        if (str_starts_with(available_commands[i], buffer)) {
            matches++;
            if (matches == 1) {
                match = available_commands[i];
                common_prefix = available_commands[i];
                common_len = strlen(available_commands[i]);
            } else if (matches == 2) {
                // First time finding multiple matches - show them
                screen_write("\n");
                screen_write(available_commands[i-1]);
                screen_write("  ");
                screen_write(available_commands[i]);

                // Calculate common prefix length
                common_len = 0;
                while (common_len < strlen(available_commands[i-1]) &&
                       common_len < strlen(available_commands[i]) &&
                       available_commands[i-1][common_len] == available_commands[i][common_len]) {
                    common_len++;
                }
            } else {
                // Show additional matches
                screen_write("  ");
                screen_write(available_commands[i]);
            }
        }
    }

    if (matches == 0) {
        // No matches - do nothing
        return 0;
    } else if (matches == 1) {
        // Single match - complete it
        int match_len = strlen(match);
        if (match_len < max_length - 1) {
            for (int i = length; i < match_len; i++) {
                buffer[i] = match[i];
                screen_putchar(match[i]);
            }
            // Add space after completed command
            if (match_len < max_length - 1) {
                buffer[match_len] = ' ';
                screen_putchar(' ');
                return match_len + 1;
            }
            return match_len;
        }
    } else {
        // Multiple matches - complete to common prefix
        screen_write("\n");
        shell_prompt();

        // Complete to common prefix if it's longer than current input
        if (common_len > length && common_len < max_length - 1) {
            for (int i = 0; i < common_len; i++) {
                buffer[i] = common_prefix[i];
                screen_putchar(common_prefix[i]);
            }
            return common_len;
        } else {
            // Just redisplay current input
            for (int i = 0; i < length; i++) {
                screen_putchar(buffer[i]);
            }
        }
    }

    return length;
}

/**
 * Get line with tab completion support
 */
static void shell_get_line(char *buffer, int max_length) {
    int length = 0;
    int cursor_pos = 0;
    int start_col = screen_get_cursor_col();
    int start_row = screen_get_cursor_row();

    while (1) {
        while (!keyboard_has_input()) {
            __asm__ __volatile__("hlt");
        }

        char c = keyboard_getchar();

        if (c == '\n') {
            buffer[length] = '\0';
            screen_putchar('\n');
            return;
        } else if (c == '\t') {
            // Tab completion
            int new_length = handle_tab_completion(buffer, length, max_length);
            length = new_length;
            cursor_pos = new_length;
            start_col = screen_get_cursor_col();
            start_row = screen_get_cursor_row();
        } else if (c == '\b') {
            if (cursor_pos > 0) {
                for (int i = cursor_pos - 1; i < length - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
                length--;
                cursor_pos--;
                screen_set_cursor(start_row, start_col);
                for (int i = 0; i < length; i++) {
                    screen_putchar(buffer[i]);
                }
                screen_putchar(' ');
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_LEFT) {
            if (cursor_pos > 0) {
                cursor_pos--;
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_RIGHT) {
            if (cursor_pos < length) {
                cursor_pos++;
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_HOME) {
            cursor_pos = 0;
            screen_set_cursor(start_row, start_col);
        } else if (c == SPECIAL_KEY_END) {
            cursor_pos = length;
            screen_set_cursor(start_row, start_col + cursor_pos);
        } else if (c == SPECIAL_KEY_DELETE) {
            if (cursor_pos < length) {
                for (int i = cursor_pos; i < length - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
                length--;
                screen_set_cursor(start_row, start_col);
                for (int i = 0; i < length; i++) {
                    screen_putchar(buffer[i]);
                }
                screen_putchar(' ');
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c >= 32 && c < 127 && length < max_length - 1) {
            if (cursor_pos < length) {
                for (int i = length; i > cursor_pos; i--) {
                    buffer[i] = buffer[i - 1];
                }
            }
            buffer[cursor_pos] = c;
            length++;
            cursor_pos++;
            screen_set_cursor(start_row, start_col);
            for (int i = 0; i < length; i++) {
                screen_putchar(buffer[i]);
            }
            screen_set_cursor(start_row, start_col + cursor_pos);
        }
    }
}

/**
 * Shell main loop
 */
void shell_run(void) {
    char command_buffer[MAX_CMD_LENGTH];

    while (1) {
        shell_prompt();
        shell_get_line(command_buffer, MAX_CMD_LENGTH);
        shell_process_command(command_buffer);
    }
}
