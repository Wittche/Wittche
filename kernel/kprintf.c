/**
 * Kernel Printf Implementation
 *
 * Provides printf-style formatted output for the kernel
 */

#include "../include/kprintf.h"
#include "../include/screen.h"
#include "../include/string.h"

// Variable argument list support
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

/**
 * Convert unsigned integer to string in given base
 */
static void utoa(uint32_t value, char *str, int base, int uppercase) {
    char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    char temp[32];
    int i = 0;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (value > 0) {
        temp[i++] = digits[value % base];
        value /= base;
    }

    int j = 0;
    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

/**
 * Convert signed integer to string
 */
static void itoa_signed(int32_t value, char *str) {
    if (value < 0) {
        str[0] = '-';
        utoa(-value, str + 1, 10, 0);
    } else {
        utoa(value, str, 10, 0);
    }
}

/**
 * Internal printf implementation
 */
static void kprintf_internal(uint8_t color, int use_color, const char *format, va_list args) {
    char buffer[32];

    while (*format) {
        if (*format == '%') {
            format++;

            switch (*format) {
                case 'd':
                case 'i': {
                    // Signed decimal integer
                    int32_t val = va_arg(args, int32_t);
                    itoa_signed(val, buffer);
                    if (use_color) {
                        screen_write_color(buffer, color);
                    } else {
                        screen_write(buffer);
                    }
                    break;
                }

                case 'u': {
                    // Unsigned decimal integer
                    uint32_t val = va_arg(args, uint32_t);
                    utoa(val, buffer, 10, 0);
                    if (use_color) {
                        screen_write_color(buffer, color);
                    } else {
                        screen_write(buffer);
                    }
                    break;
                }

                case 'x': {
                    // Hexadecimal (lowercase)
                    uint32_t val = va_arg(args, uint32_t);
                    utoa(val, buffer, 16, 0);
                    if (use_color) {
                        screen_write_color(buffer, color);
                    } else {
                        screen_write(buffer);
                    }
                    break;
                }

                case 'X': {
                    // Hexadecimal (uppercase)
                    uint32_t val = va_arg(args, uint32_t);
                    utoa(val, buffer, 16, 1);
                    if (use_color) {
                        screen_write_color(buffer, color);
                    } else {
                        screen_write(buffer);
                    }
                    break;
                }

                case 'p': {
                    // Pointer (0x prefix + hex)
                    uint32_t val = va_arg(args, uint32_t);
                    if (use_color) {
                        screen_write_color("0x", color);
                    } else {
                        screen_write("0x");
                    }
                    utoa(val, buffer, 16, 0);
                    if (use_color) {
                        screen_write_color(buffer, color);
                    } else {
                        screen_write(buffer);
                    }
                    break;
                }

                case 'c': {
                    // Character
                    char c = (char)va_arg(args, int);
                    if (use_color) {
                        char temp[2] = {c, '\0'};
                        screen_write_color(temp, color);
                    } else {
                        screen_putchar(c);
                    }
                    break;
                }

                case 's': {
                    // String
                    const char *s = va_arg(args, const char *);
                    if (s == NULL) {
                        s = "(null)";
                    }
                    if (use_color) {
                        screen_write_color(s, color);
                    } else {
                        screen_write(s);
                    }
                    break;
                }

                case '%': {
                    // Literal %
                    if (use_color) {
                        screen_write_color("%", color);
                    } else {
                        screen_putchar('%');
                    }
                    break;
                }

                default: {
                    // Unknown format specifier - print as-is
                    if (use_color) {
                        char temp[3] = {'%', *format, '\0'};
                        screen_write_color(temp, color);
                    } else {
                        screen_putchar('%');
                        screen_putchar(*format);
                    }
                    break;
                }
            }
            format++;
        } else {
            // Regular character
            if (use_color) {
                char temp[2] = {*format, '\0'};
                screen_write_color(temp, color);
            } else {
                screen_putchar(*format);
            }
            format++;
        }
    }
}

/**
 * Print formatted string to screen
 */
void kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    kprintf_internal(0, 0, format, args);
    va_end(args);
}

/**
 * Print formatted string with color
 */
void kprintf_color(uint8_t color, const char *format, ...) {
    va_list args;
    va_start(args, format);
    kprintf_internal(color, 1, format, args);
    va_end(args);
}
