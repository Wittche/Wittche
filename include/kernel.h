// Kernel header file
#ifndef KERNEL_H
#define KERNEL_H

// Function declarations
void kernel_main();
void clear_screen();
void print_string(const char *str);
void print_char(char c, unsigned char color, int row, int col);
void print_hex(unsigned int num);

// Utility functions
int strlen(const char *str);
void *memset(void *ptr, int value, unsigned int num);
void *memcpy(void *dest, const void *src, unsigned int num);

#endif // KERNEL_H
