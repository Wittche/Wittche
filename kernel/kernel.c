// Main kernel code
#include "../include/kernel.h"
#include "../include/idt.h"
#include "../include/keyboard.h"
#include "../include/types.h"

// VGA text mode buffer
#define VIDEO_MEMORY 0xB8000
#define WHITE_ON_BLACK 0x0F

// Screen dimensions (80x25)
#define MAX_ROWS 25
#define MAX_COLS 80

// Global variables for cursor position
static unsigned int cursor_row = 0;
static unsigned int cursor_col = 0;

// Function to write a character to screen
void print_char(char c, unsigned char color, int row, int col) {
    unsigned char *video_memory = (unsigned char *)VIDEO_MEMORY;
    int offset = 2 * (row * MAX_COLS + col);
    video_memory[offset] = c;
    video_memory[offset + 1] = color;
}

// Function to clear the screen
void clear_screen() {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            print_char(' ', WHITE_ON_BLACK, row, col);
        }
    }
    cursor_row = 0;
    cursor_col = 0;
}

// Function to print a string
void print_string(const char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\n') {
            cursor_col = 0;
            cursor_row++;
        } else if (str[i] == '\b') {
            // Handle backspace
            if (cursor_col > 0) {
                cursor_col--;
            } else if (cursor_row > 0) {
                cursor_row--;
                cursor_col = MAX_COLS - 1;
            }
        } else if (str[i] == '\t') {
            // Handle tab (align to 4 spaces)
            cursor_col = (cursor_col + 4) & ~3;
            if (cursor_col >= MAX_COLS) {
                cursor_col = 0;
                cursor_row++;
            }
        } else {
            print_char(str[i], WHITE_ON_BLACK, cursor_row, cursor_col);
            cursor_col++;
            if (cursor_col >= MAX_COLS) {
                cursor_col = 0;
                cursor_row++;
            }
        }

        // Scroll if necessary
        if (cursor_row >= MAX_ROWS) {
            cursor_row = MAX_ROWS - 1;
            // Simple scroll: clear screen (in a real OS, we'd shift all lines up)
            // For now, just wrap to top
            cursor_row = 0;
        }

        i++;
    }
}

// Function to print a number in hexadecimal
void print_hex(unsigned int num) {
    char hex_chars[] = "0123456789ABCDEF";
    char hex_string[11] = "0x00000000";

    for (int i = 9; i >= 2; i--) {
        hex_string[i] = hex_chars[num & 0xF];
        num >>= 4;
    }

    print_string(hex_string);
}

// Simple string length function
int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Simple memory set function
void *memset(void *ptr, int value, unsigned int num) {
    unsigned char *p = (unsigned char *)ptr;
    for (unsigned int i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

// Simple memory copy function
void *memcpy(void *dest, const void *src, unsigned int num) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (unsigned int i = 0; i < num; i++) {
        d[i] = s[i];
    }
    return dest;
}

// Simple string compare function
int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

// Simple string copy function
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

// Process shell command
void process_command(char *command) {
    if (strcmp(command, "help") == 0) {
        print_string("\nAvailable commands:\n");
        print_string("  help    - Show this help message\n");
        print_string("  clear   - Clear the screen\n");
        print_string("  about   - Show system information\n");
        print_string("  echo    - Echo a message\n");
    } else if (strcmp(command, "clear") == 0) {
        clear_screen();
    } else if (strcmp(command, "about") == 0) {
        print_string("\nWittche Operating System v0.2\n");
        print_string("A simple x86 operating system\n");
        print_string("Architecture: x86 (32-bit)\n");
        print_string("Features: IDT, Interrupts, Keyboard Input\n");
    } else if (command[0] == 'e' && command[1] == 'c' && command[2] == 'h' &&
               command[3] == 'o' && command[4] == ' ') {
        print_string("\n");
        print_string(&command[5]);
        print_string("\n");
    } else if (command[0] == '\0') {
        // Empty command, just print newline
        print_string("\n");
    } else {
        print_string("\nUnknown command: ");
        print_string(command);
        print_string("\nType 'help' for available commands.\n");
    }
}

// Main kernel function
void kernel_main() {
    // Clear the screen
    clear_screen();

    // Print welcome message
    print_string("Wittche Operating System v0.2\n");
    print_string("============================\n\n");

    // Initialize IDT and interrupts
    idt_init();

    // Initialize keyboard driver
    keyboard_init();

    print_string("\nWelcome to Wittche OS!\n");
    print_string("This is a simple x86 operating system kernel.\n");
    print_string("Type 'help' for available commands.\n\n");

    // Simple shell loop
    char command_buffer[256];

    while (1) {
        print_string("> ");
        keyboard_get_line(command_buffer, sizeof(command_buffer));
        process_command(command_buffer);
    }
}
