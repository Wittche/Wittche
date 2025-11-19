// Keyboard driver implementation
#include "../include/keyboard.h"
#include "../include/ports.h"
#include "../include/screen.h"
#include "../include/types.h"

// Keyboard state
static int shift_pressed = 0;
static int caps_lock = 0;
static int ctrl_pressed = 0;
static int extended_key = 0;  // Track E0 prefix for extended scancodes

// Keyboard buffer
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int buffer_read_pos = 0;
static int buffer_write_pos = 0;

// US QWERTY keyboard layout scancode to ASCII
static char scancode_to_ascii_lower[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static char scancode_to_ascii_upper[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Add character to keyboard buffer
static void keyboard_buffer_add(char c) {
    int next_pos = (buffer_write_pos + 1) % KEYBOARD_BUFFER_SIZE;
    if (next_pos != buffer_read_pos) {
        keyboard_buffer[buffer_write_pos] = c;
        buffer_write_pos = next_pos;
    }
}

// Get character from keyboard buffer
char keyboard_getchar(void) {
    if (buffer_read_pos == buffer_write_pos) {
        return 0;  // Buffer empty
    }

    char c = keyboard_buffer[buffer_read_pos];
    buffer_read_pos = (buffer_read_pos + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Check if keyboard has input
int keyboard_has_input(void) {
    return buffer_read_pos != buffer_write_pos;
}

// Keyboard interrupt handler
void keyboard_handler(void) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);

    // Check for extended scancode prefix (E0)
    if (scancode == 0xE0) {
        extended_key = 1;
        return;
    }

    // Check if key release (bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Remove release bit

        // Handle special key releases
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 0;
        } else if (scancode == KEY_LCTRL) {
            ctrl_pressed = 0;
        }

        extended_key = 0;  // Reset extended key flag
        return;
    }

    // Handle extended scancodes (arrow keys, etc.)
    if (extended_key) {
        extended_key = 0;  // Reset flag

        // Map extended scancodes to special internal codes
        char special_key = 0;
        switch (scancode) {
            case KEY_LEFT:
                special_key = SPECIAL_KEY_LEFT;
                break;
            case KEY_RIGHT:
                special_key = SPECIAL_KEY_RIGHT;
                break;
            case KEY_UP:
                special_key = SPECIAL_KEY_UP;
                break;
            case KEY_DOWN:
                special_key = SPECIAL_KEY_DOWN;
                break;
            case KEY_HOME:
                special_key = SPECIAL_KEY_HOME;
                break;
            case KEY_END:
                special_key = SPECIAL_KEY_END;
                break;
            case KEY_DELETE:
                special_key = SPECIAL_KEY_DELETE;
                break;
        }

        // Add special key to buffer if recognized
        if (special_key != 0) {
            keyboard_buffer_add(special_key);
        }
        return;
    }

    // Handle special keys
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        shift_pressed = 1;
        return;
    } else if (scancode == KEY_CAPS) {
        caps_lock = !caps_lock;
        return;
    } else if (scancode == KEY_LCTRL) {
        ctrl_pressed = 1;
        return;
    }

    // Convert scancode to ASCII
    char ascii = 0;
    if (scancode < sizeof(scancode_to_ascii_lower)) {
        int use_upper = shift_pressed ^ caps_lock;  // XOR for shift and caps lock

        if (use_upper && scancode < sizeof(scancode_to_ascii_upper)) {
            ascii = scancode_to_ascii_upper[scancode];
        } else {
            ascii = scancode_to_ascii_lower[scancode];
        }
    }

    // Add to buffer if valid character
    if (ascii != 0) {
        keyboard_buffer_add(ascii);
        // Note: keyboard_get_line() handles screen output for proper cursor support
    }
}

// Initialize keyboard
void keyboard_init(void) {
    buffer_read_pos = 0;
    buffer_write_pos = 0;
    shift_pressed = 0;
    caps_lock = 0;
    ctrl_pressed = 0;

    screen_write_color("[KEYBOARD] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("Keyboard driver initialized\n");
}

// Get a line of input from keyboard with cursor support
void keyboard_get_line(char *buffer, int max_length) {
    int length = 0;      // Total characters in buffer
    int cursor_pos = 0;  // Current cursor position (0 to length)
    int start_col = screen_get_cursor_col();  // Remember where input started
    int start_row = screen_get_cursor_row();

    while (1) {
        while (!keyboard_has_input()) {
            __asm__ __volatile__("hlt");  // Wait for interrupt
        }

        char c = keyboard_getchar();

        if (c == '\n') {
            // Enter pressed - return the line
            buffer[length] = '\0';
            screen_putchar('\n');
            return;
        } else if (c == '\b') {
            // Backspace - delete character before cursor
            if (cursor_pos > 0) {
                // Shift everything after cursor one position left
                for (int i = cursor_pos - 1; i < length - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
                length--;
                cursor_pos--;

                // Redraw the line from cursor position
                screen_set_cursor(start_row, start_col);
                for (int i = 0; i < length; i++) {
                    screen_putchar(buffer[i]);
                }
                screen_putchar(' ');  // Clear last character
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_LEFT) {
            // Move cursor left
            if (cursor_pos > 0) {
                cursor_pos--;
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_RIGHT) {
            // Move cursor right
            if (cursor_pos < length) {
                cursor_pos++;
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c == SPECIAL_KEY_HOME) {
            // Jump to beginning
            cursor_pos = 0;
            screen_set_cursor(start_row, start_col);
        } else if (c == SPECIAL_KEY_END) {
            // Jump to end
            cursor_pos = length;
            screen_set_cursor(start_row, start_col + cursor_pos);
        } else if (c == SPECIAL_KEY_DELETE) {
            // Delete character at cursor
            if (cursor_pos < length) {
                // Shift everything after cursor one position left
                for (int i = cursor_pos; i < length - 1; i++) {
                    buffer[i] = buffer[i + 1];
                }
                length--;

                // Redraw the line from cursor position
                screen_set_cursor(start_row, start_col);
                for (int i = 0; i < length; i++) {
                    screen_putchar(buffer[i]);
                }
                screen_putchar(' ');  // Clear last character
                screen_set_cursor(start_row, start_col + cursor_pos);
            }
        } else if (c >= 32 && c < 127 && length < max_length - 1) {
            // Printable character - insert at cursor position
            if (cursor_pos < length) {
                // Shift everything after cursor one position right
                for (int i = length; i > cursor_pos; i--) {
                    buffer[i] = buffer[i - 1];
                }
            }

            // Insert new character
            buffer[cursor_pos] = c;
            length++;
            cursor_pos++;

            // Redraw from cursor position to end
            screen_set_cursor(start_row, start_col);
            for (int i = 0; i < length; i++) {
                screen_putchar(buffer[i]);
            }
            screen_set_cursor(start_row, start_col + cursor_pos);
        }
        // Ignore special keys we don't handle (UP, DOWN for now)
    }
}
