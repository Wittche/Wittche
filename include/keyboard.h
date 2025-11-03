// Keyboard driver header
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Keyboard data port
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Special keys
#define KEY_BACKSPACE 0x0E
#define KEY_ENTER 0x1C
#define KEY_LSHIFT 0x2A
#define KEY_RSHIFT 0x36
#define KEY_LCTRL 0x1D
#define KEY_ALT 0x38
#define KEY_CAPS 0x3A

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Function declarations
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
int keyboard_has_input(void);
void keyboard_get_line(char *buffer, int max_length);

#endif // KEYBOARD_H
