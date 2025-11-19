/**
 * Kernel Printf - Formatted Output for Wittche OS
 *
 * Provides printf-style formatted output for kernel messages
 */

#ifndef KPRINTF_H
#define KPRINTF_H

#include "types.h"

/**
 * Print formatted string to screen
 * Supported format specifiers:
 *   %d, %i - signed decimal integer
 *   %u - unsigned decimal integer
 *   %x - hexadecimal (lowercase)
 *   %X - hexadecimal (uppercase)
 *   %c - character
 *   %s - string
 *   %p - pointer (hexadecimal with 0x prefix)
 *   %% - literal %
 */
void kprintf(const char *format, ...);

/**
 * Print formatted string with color
 */
void kprintf_color(uint8_t color, const char *format, ...);

#endif // KPRINTF_H
