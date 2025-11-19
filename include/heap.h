// Kernel Heap Manager header
#ifndef HEAP_H
#define HEAP_H

#include "types.h"

// Heap configuration
#define HEAP_START 0x00200000       // 2MB (after kernel)
#define HEAP_SIZE (4 * 1024 * 1024) // 4MB heap
#define HEAP_END (HEAP_START + HEAP_SIZE)

// Magic number for heap block validation
#define HEAP_MAGIC 0xDEADBEEF

// Minimum allocation size (including header)
#define MIN_BLOCK_SIZE 32

/**
 * Heap block header structure
 */
typedef struct heap_block {
    uint32_t size;              // Block size (including header)
    uint32_t is_free;           // 1 = free, 0 = allocated
    uint32_t magic;             // Magic number for validation
    struct heap_block *next;    // Next block in list
} heap_block_t;

/**
 * Kernel heap functions
 */

// Initialize kernel heap
void heap_init(void);

// Allocate memory from kernel heap
void *kmalloc(uint32_t size);

// Free memory back to kernel heap
void kfree(void *ptr);

// Reallocate memory block
void *krealloc(void *ptr, uint32_t new_size);

// Allocate and zero memory
void *kcalloc(uint32_t num, uint32_t size);

// Get heap statistics
uint32_t heap_get_total_size(void);
uint32_t heap_get_used_size(void);
uint32_t heap_get_free_size(void);
uint32_t heap_get_block_count(void);
uint32_t heap_get_free_block_count(void);

#endif // HEAP_H
