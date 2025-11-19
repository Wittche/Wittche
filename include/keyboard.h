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
#define KEY_UP 0x48
#define KEY_DOWN 0x50
#define KEY_LEFT 0x4B
#define KEY_RIGHT 0x4D
#define KEY_HOME 0x47
#define KEY_END 0x4F
#define KEY_DELETE 0x53

// Special key codes (internal use)
#define SPECIAL_KEY_UP 0x01
#define SPECIAL_KEY_DOWN 0x02
#define SPECIAL_KEY_LEFT 0x03
#define SPECIAL_KEY_RIGHT 0x04
#define SPECIAL_KEY_HOME 0x05
#define SPECIAL_KEY_END 0x06
#define SPECIAL_KEY_DELETE 0x07

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Function declarations
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
int keyboard_has_input(void);
void keyboard_get_line(char *buffer, int max_length);

#endif // KEYBOARD_H
