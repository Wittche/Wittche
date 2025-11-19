// Kernel Heap Manager implementation
#include "../include/heap.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/string.h"

// Head of the heap block list
static heap_block_t *heap_head = NULL;

/**
 * Align size to 4-byte boundary
 */
static uint32_t align_size(uint32_t size) {
    return (size + 3) & ~3;
}

/**
 * Coalesce adjacent free blocks
 */
static void coalesce_free_blocks(void) {
    heap_block_t *current = heap_head;

    while (current && current->next) {
        // If current and next are both free and adjacent
        if (current->is_free && current->next->is_free) {
            uint32_t current_end = (uint32_t)current + current->size;
            uint32_t next_start = (uint32_t)current->next;

            if (current_end == next_start) {
                // Merge blocks
                current->size += current->next->size;
                current->next = current->next->next;
                continue;  // Check again from same position
            }
        }
        current = current->next;
    }
}

/**
 * Initialize kernel heap
 */
void heap_init(void) {
    // Create initial free block spanning entire heap
    heap_head = (heap_block_t *)HEAP_START;
    heap_head->size = HEAP_SIZE;
    heap_head->is_free = 1;
    heap_head->magic = HEAP_MAGIC;
    heap_head->next = NULL;

    screen_write_color("[HEAP] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("Kernel heap initialized\n");

    kprintf("       Heap Start: 0x%x\n", HEAP_START);
    kprintf("       Heap Size: %d MB\n", HEAP_SIZE / (1024 * 1024));
    kprintf("       Heap End: 0x%x\n", HEAP_END);
}

/**
 * Allocate memory from kernel heap
 */
void *kmalloc(uint32_t size) {
    if (size == 0) return NULL;

    // Align size and add header
    uint32_t aligned_size = align_size(size);
    uint32_t total_size = aligned_size + sizeof(heap_block_t);

    // Ensure minimum block size
    if (total_size < MIN_BLOCK_SIZE) {
        total_size = MIN_BLOCK_SIZE;
    }

    // Find first free block that fits (first-fit algorithm)
    heap_block_t *current = heap_head;
    while (current) {
        // Check magic number
        if (current->magic != HEAP_MAGIC) {
            kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                         "[HEAP ERROR] Corrupted heap block at 0x%x\n", current);
            return NULL;
        }

        if (current->is_free && current->size >= total_size) {
            // Found suitable block

            // Split block if remainder is large enough
            if (current->size >= total_size + MIN_BLOCK_SIZE) {
                heap_block_t *new_block = (heap_block_t *)((uint32_t)current + total_size);
                new_block->size = current->size - total_size;
                new_block->is_free = 1;
                new_block->magic = HEAP_MAGIC;
                new_block->next = current->next;

                current->size = total_size;
                current->next = new_block;
            }

            // Mark block as allocated
            current->is_free = 0;

            // Return pointer after header
            return (void *)((uint32_t)current + sizeof(heap_block_t));
        }

        current = current->next;
    }

    // No suitable block found
    return NULL;
}

/**
 * Free memory back to kernel heap
 */
void kfree(void *ptr) {
    if (!ptr) return;

    // Get block header
    heap_block_t *block = (heap_block_t *)((uint32_t)ptr - sizeof(heap_block_t));

    // Validate magic number
    if (block->magic != HEAP_MAGIC) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[HEAP ERROR] Invalid free - bad magic at 0x%x\n", block);
        return;
    }

    // Validate block is within heap
    if ((uint32_t)block < HEAP_START || (uint32_t)block >= HEAP_END) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[HEAP ERROR] Free outside heap bounds: 0x%x\n", block);
        return;
    }

    // Check if already free (double free)
    if (block->is_free) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[HEAP WARNING] Double free detected at 0x%x\n", block);
        return;
    }

    // Mark as free
    block->is_free = 1;

    // Coalesce adjacent free blocks
    coalesce_free_blocks();
}

/**
 * Reallocate memory block
 */
void *krealloc(void *ptr, uint32_t new_size) {
    if (!ptr) {
        return kmalloc(new_size);
    }

    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }

    // Get current block
    heap_block_t *block = (heap_block_t *)((uint32_t)ptr - sizeof(heap_block_t));

    // Validate magic
    if (block->magic != HEAP_MAGIC) {
        return NULL;
    }

    uint32_t current_size = block->size - sizeof(heap_block_t);

    // If new size fits in current block, just return same pointer
    if (new_size <= current_size) {
        return ptr;
    }

    // Allocate new block
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) {
        return NULL;
    }

    // Copy old data
    memcpy(new_ptr, ptr, current_size);

    // Free old block
    kfree(ptr);

    return new_ptr;
}

/**
 * Allocate and zero memory
 */
void *kcalloc(uint32_t num, uint32_t size) {
    uint32_t total = num * size;
    void *ptr = kmalloc(total);

    if (ptr) {
        memset(ptr, 0, total);
    }

    return ptr;
}

/**
 * Heap statistics functions
 */
uint32_t heap_get_total_size(void) {
    return HEAP_SIZE;
}

uint32_t heap_get_used_size(void) {
    uint32_t used = 0;
    heap_block_t *current = heap_head;

    while (current) {
        if (!current->is_free) {
            used += current->size;
        }
        current = current->next;
    }

    return used;
}

uint32_t heap_get_free_size(void) {
    uint32_t free = 0;
    heap_block_t *current = heap_head;

    while (current) {
        if (current->is_free) {
            free += current->size;
        }
        current = current->next;
    }

    return free;
}

uint32_t heap_get_block_count(void) {
    uint32_t count = 0;
    heap_block_t *current = heap_head;

    while (current) {
        count++;
        current = current->next;
    }

    return count;
}

uint32_t heap_get_free_block_count(void) {
    uint32_t count = 0;
    heap_block_t *current = heap_head;

    while (current) {
        if (current->is_free) {
            count++;
        }
        current = current->next;
    }

    return count;
}
