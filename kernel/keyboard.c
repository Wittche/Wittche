// Keyboard driver implementation
#include "../include/keyboard.h"
#include "../include/ports.h"
#include "../include/kernel.h"
#include "../include/types.h"

// Keyboard state
static int shift_pressed = 0;
static int caps_lock = 0;
static int ctrl_pressed = 0;

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

    // Check if key release (bit 7 set)
    if (scancode & 0x80) {
        scancode &= 0x7F;  // Remove release bit

        // Handle special key releases
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 0;
        } else if (scancode == KEY_LCTRL) {
            ctrl_pressed = 0;
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

        // Echo character to screen
        if (ascii == '\b') {
            print_string("\b \b");  // Backspace: move back, space, move back
        } else if (ascii == '\n') {
            print_char('\n', 0x0F, 0, 0);  // Newline
        } else {
            char str[2] = {ascii, '\0'};
            print_string(str);
        }
    }
}

// Initialize keyboard
void keyboard_init(void) {
    buffer_read_pos = 0;
    buffer_write_pos = 0;
    shift_pressed = 0;
    caps_lock = 0;
    ctrl_pressed = 0;

    print_string("[KEYBOARD] Keyboard driver initialized\n");
}

// Get a line of input from keyboard
void keyboard_get_line(char *buffer, int max_length) {
    int pos = 0;

    while (1) {
        while (!keyboard_has_input()) {
            __asm__ __volatile__("hlt");  // Wait for interrupt
        }

        char c = keyboard_getchar();

        if (c == '\n') {
            buffer[pos] = '\0';
            return;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
            }
        } else if (pos < max_length - 1) {
            buffer[pos++] = c;
        }
    }
}
