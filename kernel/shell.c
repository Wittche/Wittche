// Shell implementation
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/keyboard.h"
#include "../include/timer.h"
#include "../include/kprintf.h"
#include "../include/types.h"

// Command history
#define MAX_HISTORY 10
#define MAX_CMD_LENGTH 256

static char command_history[MAX_HISTORY][MAX_CMD_LENGTH];
static int history_count = 0;

/**
 * Display welcome banner
 */
void shell_display_banner(void) {
    screen_write_color("\n", DEFAULT_COLOR);
    screen_write_color("===========================================\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write_color(" Wittche Operating System v0.5\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
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
    screen_write("       - Display memory information\n");
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
    screen_write_color("Wittche Operating System v0.5\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_color("===============================\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write("\n");
    screen_write("A simple x86 operating system for educational purposes.\n\n");

    screen_write_color("Features:\n", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("  - 32-bit protected mode kernel\n");
    screen_write("  - Hardware interrupt handling (IDT)\n");
    screen_write("  - Programmable Interval Timer (PIT)\n");
    screen_write("  - PS/2 keyboard driver\n");
    screen_write("  - VGA text mode with hardware cursor\n");
    screen_write("  - Proper screen scrolling\n");
    screen_write("  - Printf-style formatted output (kprintf)\n");
    screen_write("  - Interactive shell with command parsing\n");
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
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "0.5.0\n");
    kprintf("  Codename:    ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "Enhanced UX\n");
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
    kprintf("  Available:       ");
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "Managed by kernel\n");
    kprintf("  Note:            ");
    kprintf_color(MAKE_COLOR(COLOR_DARK_GREY, COLOR_BLACK), "Dynamic memory allocator coming soon!\n");
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
 * Shell main loop
 */
void shell_run(void) {
    char command_buffer[MAX_CMD_LENGTH];

    // CRITICAL DEBUG: Direct VGA write to bypass any screen_write issues
    __asm__ __volatile__("cli");  // Disable interrupts
    volatile unsigned short *vga = (unsigned short *)0xB8000;
    // Write at position 400 (row 5, col 0) to be visible
    const char *msg = ">>>SHELL_RUN_ENTERED<<<";
    for (int i = 0; msg[i] != '\0'; i++) {
        vga[400 + i] = 0x4F00 | msg[i];  // White on red - very visible!
    }
    __asm__ __volatile__("sti");  // Re-enable interrupts

    // Wait a bit so user can see the message
    for (volatile int delay = 0; delay < 10000000; delay++);

    screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write("Entering shell_run() main loop\n");

    while (1) {
        screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write("Calling shell_prompt()...\n");

        shell_prompt();

        screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write("Prompt displayed, waiting for input...\n");

        keyboard_get_line(command_buffer, MAX_CMD_LENGTH);

        screen_write_color("[DEBUG] ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write("Got input, processing command...\n");

        shell_process_command(command_buffer);
    }
}
