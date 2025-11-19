// Screen/VGA driver implementation
#include "../include/screen.h"
#include "../include/ports.h"
#include "../include/types.h"
#include "../include/string.h"

// VGA text mode buffer
#define VIDEO_MEMORY 0xB8000

// VGA hardware text mode cursor ports
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5
#define VGA_CURSOR_HIGH 14
#define VGA_CURSOR_LOW 15

// Current cursor position and color
static int cursor_row = 0;
static int cursor_col = 0;
static uint8_t current_color = DEFAULT_COLOR;

// Video memory pointer
static uint16_t *video_memory = (uint16_t *)VIDEO_MEMORY;

/**
 * Initialize the screen driver
 */
void screen_init(void) {
    // Set initial state
    cursor_row = 0;
    cursor_col = 0;
    current_color = DEFAULT_COLOR;

    // Clear entire screen with spaces
    uint16_t blank = (DEFAULT_COLOR << 8) | ' ';
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video_memory[i] = blank;
    }

    // Reset cursor to top-left
    cursor_row = 0;
    cursor_col = 0;
    screen_update_cursor();
}

/**
 * Update hardware cursor position
 */
void screen_update_cursor(void) {
    uint16_t position = cursor_row * SCREEN_WIDTH + cursor_col;

    // Send high byte
    outb(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    outb(VGA_DATA_REGISTER, (position >> 8) & 0xFF);

    // Send low byte
    outb(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    outb(VGA_DATA_REGISTER, position & 0xFF);
}

/**
 * Scroll the screen up by one line
 */
void screen_scroll(void) {
    // Move all lines up by one
    for (int row = 0; row < SCREEN_HEIGHT - 1; row++) {
        for (int col = 0; col < SCREEN_WIDTH; col++) {
            int src_pos = (row + 1) * SCREEN_WIDTH + col;
            int dst_pos = row * SCREEN_WIDTH + col;
            video_memory[dst_pos] = video_memory[src_pos];
        }
    }

    // Clear the last line
    uint16_t blank = (current_color << 8) | ' ';
    for (int col = 0; col < SCREEN_WIDTH; col++) {
        int pos = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH + col;
        video_memory[pos] = blank;
    }

    // Move cursor to last line
    cursor_row = SCREEN_HEIGHT - 1;
    cursor_col = 0;
}

/**
 * Clear the entire screen
 */
void screen_clear(void) {
    uint16_t blank = (current_color << 8) | ' ';

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        video_memory[i] = blank;
    }

    cursor_row = 0;
    cursor_col = 0;
    screen_update_cursor();
}

/**
 * Put a single character on screen
 */
void screen_putchar(char c) {
    if (c == '\n') {
        // Newline
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\r') {
        // Carriage return
        cursor_col = 0;
    } else if (c == '\t') {
        // Tab - align to next 4-space boundary
        cursor_col = (cursor_col + 4) & ~3;
    } else if (c == '\b') {
        // Backspace
        if (cursor_col > 0) {
            cursor_col--;
            int position = cursor_row * SCREEN_WIDTH + cursor_col;
            video_memory[position] = (current_color << 8) | ' ';
        } else if (cursor_row > 0) {
            cursor_row--;
            cursor_col = SCREEN_WIDTH - 1;
        }
    } else {
        // Normal character
        int position = cursor_row * SCREEN_WIDTH + cursor_col;
        video_memory[position] = (current_color << 8) | c;
        cursor_col++;
    }

    // Handle line wrap
    if (cursor_col >= SCREEN_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }

    // Handle scrolling
    if (cursor_row >= SCREEN_HEIGHT) {
        screen_scroll();
    }

    screen_update_cursor();
}

/**
 * Write a string to screen
 */
void screen_write(const char *str) {
    if (!str) return;

    for (int i = 0; str[i] != '\0'; i++) {
        screen_putchar(str[i]);
    }
}

/**
 * Write a string with specific color
 */
void screen_write_color(const char *str, uint8_t color) {
    uint8_t old_color = current_color;
    current_color = color;
    screen_write(str);
    current_color = old_color;
}

/**
 * Write a hexadecimal number
 */
void screen_write_hex(uint32_t num) {
    char hex_chars[] = "0123456789ABCDEF";
    char buffer[11] = "0x00000000";

    for (int i = 9; i >= 2; i--) {
        buffer[i] = hex_chars[num & 0xF];
        num >>= 4;
    }

    screen_write(buffer);
}

/**
 * Write a decimal number
 */
void screen_write_dec(uint32_t num) {
    char buffer[12]; // Max 10 digits + sign + null
    int i = 0;

    if (num == 0) {
        screen_putchar('0');
        return;
    }

    // Convert number to string (in reverse)
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    // Print in correct order
    for (int j = i - 1; j >= 0; j--) {
        screen_putchar(buffer[j]);
    }
}

/**
 * Set the current text color
 */
void screen_set_color(uint8_t color) {
    current_color = color;
}

/**
 * Get current cursor row
 */
int screen_get_cursor_row(void) {
    return cursor_row;
}

/**
 * Get current cursor column
 */
int screen_get_cursor_col(void) {
    return cursor_col;
}

/**
 * Set cursor position
 */
void screen_set_cursor(int row, int col) {
    if (row >= 0 && row < SCREEN_HEIGHT && col >= 0 && col < SCREEN_WIDTH) {
        cursor_row = row;
        cursor_col = col;
        screen_update_cursor();
    }
}
