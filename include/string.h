// String utilities header
#ifndef STRING_H
#define STRING_H

#include "types.h"

// String functions
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, int n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, int n);
char *strcat(char *dest, const char *src);
char *strchr(const char *str, char c);
void str_trim(char *str);
int str_split(char *str, char delim, char **tokens, int max_tokens);
void str_to_upper(char *str);
void str_to_lower(char *str);
int str_starts_with(const char *str, const char *prefix);

// Memory functions
void *memset(void *ptr, int value, uint32_t num);
void *memcpy(void *dest, const void *src, uint32_t num);
int memcmp(const void *ptr1, const void *ptr2, uint32_t num);

// Conversion functions
int atoi(const char *str);
void itoa(int value, char *str, int base);

#endif // STRING_H
