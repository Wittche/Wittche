// String utilities implementation
#include "../include/string.h"
#include "../include/types.h"

/**
 * Get string length
 */
int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/**
 * Compare two strings
 * Returns: 0 if equal, <0 if str1 < str2, >0 if str1 > str2
 */
int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

/**
 * Compare first n characters of two strings
 */
int strncmp(const char *str1, const char *str2, int n) {
    while (n > 0 && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

/**
 * Copy string
 */
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

/**
 * Copy at most n characters
 */
char *strncpy(char *dest, const char *src, int n) {
    char *ret = dest;
    while (n > 0 && (*dest++ = *src++)) {
        n--;
    }
    while (n > 0) {
        *dest++ = '\0';
        n--;
    }
    return ret;
}

/**
 * Concatenate strings
 */
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

/**
 * Find character in string
 */
char *strchr(const char *str, char c) {
    while (*str) {
        if (*str == c) return (char *)str;
        str++;
    }
    return NULL;
}

/**
 * Trim whitespace from string (in-place)
 */
void str_trim(char *str) {
    if (!str) return;

    // Trim leading whitespace
    char *start = str;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
        start++;
    }

    // Trim trailing whitespace
    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }

    // Copy trimmed string
    int len = end - start + 1;
    if (start != str) {
        for (int i = 0; i < len; i++) {
            str[i] = start[i];
        }
    }
    str[len] = '\0';
}

/**
 * Split string by delimiter
 * Returns number of tokens found
 */
int str_split(char *str, char delim, char **tokens, int max_tokens) {
    int count = 0;
    char *current = str;

    while (*current && count < max_tokens) {
        // Skip leading delimiters
        while (*current == delim) current++;

        if (*current == '\0') break;

        // Mark start of token
        tokens[count++] = current;

        // Find end of token
        while (*current && *current != delim) current++;

        // Null terminate if not end of string
        if (*current) {
            *current = '\0';
            current++;
        }
    }

    return count;
}

/**
 * Convert string to uppercase (in-place)
 */
void str_to_upper(char *str) {
    while (*str) {
        if (*str >= 'a' && *str <= 'z') {
            *str = *str - 'a' + 'A';
        }
        str++;
    }
}

/**
 * Convert string to lowercase (in-place)
 */
void str_to_lower(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str = *str - 'A' + 'a';
        }
        str++;
    }
}

/**
 * Check if string starts with prefix
 */
int str_starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str != *prefix) return 0;
        str++;
        prefix++;
    }
    return 1;
}

/**
 * Set memory to value
 */
void *memset(void *ptr, int value, uint32_t num) {
    unsigned char *p = (unsigned char *)ptr;
    for (uint32_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}

/**
 * Copy memory
 */
void *memcpy(void *dest, const void *src, uint32_t num) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (uint32_t i = 0; i < num; i++) {
        d[i] = s[i];
    }
    return dest;
}

/**
 * Compare memory
 */
int memcmp(const void *ptr1, const void *ptr2, uint32_t num) {
    const unsigned char *p1 = (const unsigned char *)ptr1;
    const unsigned char *p2 = (const unsigned char *)ptr2;

    for (uint32_t i = 0; i < num; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

/**
 * Convert string to integer
 */
int atoi(const char *str) {
    int result = 0;
    int sign = 1;

    // Skip whitespace
    while (*str == ' ' || *str == '\t') str++;

    // Handle sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Convert digits
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return result * sign;
}

/**
 * Convert integer to string
 */
void itoa(int value, char *str, int base) {
    if (base < 2 || base > 36) {
        *str = '\0';
        return;
    }

    char *ptr = str;
    int is_negative = 0;

    // Handle negative numbers for base 10
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    // Convert to string (reverse order)
    int i = 0;
    do {
        int digit = value % base;
        ptr[i++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
        value /= base;
    } while (value > 0);

    // Add negative sign
    if (is_negative) {
        ptr[i++] = '-';
    }

    ptr[i] = '\0';

    // Reverse string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = ptr[start];
        ptr[start] = ptr[end];
        ptr[end] = temp;
        start++;
        end--;
    }
}
