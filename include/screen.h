// Screen/VGA driver header
#ifndef SCREEN_H
#define SCREEN_H

#include "types.h"

// Screen dimensions
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// VGA colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GREY 7
#define COLOR_DARK_GREY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Make a color attribute byte
#define MAKE_COLOR(fg, bg) ((bg << 4) | (fg & 0x0F))

// Default colors
#define DEFAULT_COLOR MAKE_COLOR(COLOR_LIGHT_GREY, COLOR_BLACK)

// Function declarations
void screen_init(void);
void screen_clear(void);
void screen_putchar(char c);
void screen_write(const char *str);
void screen_write_color(const char *str, uint8_t color);
void screen_write_hex(uint32_t num);
void screen_write_dec(uint32_t num);
void screen_set_color(uint8_t color);
void screen_scroll(void);
void screen_update_cursor(void);
int screen_get_cursor_row(void);
int screen_get_cursor_col(void);
void screen_set_cursor(int row, int col);

#endif // SCREEN_H
