// Physical Memory Manager implementation
#include "../include/pmm.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/string.h"

// Bitmap to track page allocation (1 bit per page)
// 4096 pages = 512 bytes for bitmap
static uint32_t page_bitmap[TOTAL_PAGES / 32];  // 128 uint32_t entries
static uint32_t used_pages = 0;

/**
 * Set a bit in the bitmap
 */
static void bitmap_set(uint32_t page_index) {
    uint32_t index = page_index / 32;
    uint32_t bit = page_index % 32;
    page_bitmap[index] |= (1 << bit);
}

/**
 * Clear a bit in the bitmap
 */
static void bitmap_clear(uint32_t page_index) {
    uint32_t index = page_index / 32;
    uint32_t bit = page_index % 32;
    page_bitmap[index] &= ~(1 << bit);
}

/**
 * Test if a bit is set in the bitmap
 */
static int bitmap_test(uint32_t page_index) {
    uint32_t index = page_index / 32;
    uint32_t bit = page_index % 32;
    return (page_bitmap[index] & (1 << bit)) != 0;
}

/**
 * Find first free page in bitmap
 */
static uint32_t find_free_page(void) {
    for (uint32_t i = 0; i < TOTAL_PAGES; i++) {
        if (!bitmap_test(i)) {
            return i;
        }
    }
    return 0xFFFFFFFF;  // No free pages
}

/**
 * Find multiple contiguous free pages
 */
static uint32_t find_free_pages(uint32_t count) {
    if (count == 0) return 0xFFFFFFFF;

    for (uint32_t i = 0; i <= TOTAL_PAGES - count; i++) {
        int found = 1;
        for (uint32_t j = 0; j < count; j++) {
            if (bitmap_test(i + j)) {
                found = 0;
                i += j;  // Skip ahead
                break;
            }
        }
        if (found) {
            return i;
        }
    }
    return 0xFFFFFFFF;  // Not enough contiguous pages
}

/**
 * Initialize physical memory manager
 */
void pmm_init(void) {
    // Clear bitmap (all pages initially free)
    memset(page_bitmap, 0, sizeof(page_bitmap));
    used_pages = 0;

    // Mark first 2MB as reserved (kernel space)
    uint32_t kernel_pages = KERNEL_END / PAGE_SIZE;  // 512 pages (2MB)
    for (uint32_t i = 0; i < kernel_pages; i++) {
        bitmap_set(i);
        used_pages++;
    }

    screen_write_color("[PMM] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("Physical Memory Manager initialized\n");

    kprintf("      Total Memory: %d MB (%d pages)\n",
            MEMORY_SIZE / (1024 * 1024), TOTAL_PAGES);
    kprintf("      Kernel Reserved: %d KB (%d pages)\n",
            KERNEL_END / 1024, kernel_pages);
    kprintf("      Available: %d MB (%d pages)\n",
            (MEMORY_SIZE - KERNEL_END) / (1024 * 1024),
            TOTAL_PAGES - kernel_pages);
}

/**
 * Allocate a physical page (4KB)
 */
uint32_t pmm_alloc_page(void) {
    uint32_t page_index = find_free_page();

    if (page_index == 0xFFFFFFFF) {
        return 0;  // Out of memory
    }

    bitmap_set(page_index);
    used_pages++;

    return page_index * PAGE_SIZE;  // Return physical address
}

/**
 * Free a physical page
 */
void pmm_free_page(uint32_t page_addr) {
    // Validate address
    if (page_addr >= MEMORY_SIZE || page_addr % PAGE_SIZE != 0) {
        return;  // Invalid address
    }

    // Don't allow freeing kernel pages
    if (page_addr < KERNEL_END) {
        return;
    }

    uint32_t page_index = page_addr / PAGE_SIZE;

    if (bitmap_test(page_index)) {
        bitmap_clear(page_index);
        used_pages--;
    }
}

/**
 * Allocate multiple contiguous pages
 */
uint32_t pmm_alloc_pages(uint32_t count) {
    if (count == 0) return 0;

    uint32_t page_index = find_free_pages(count);

    if (page_index == 0xFFFFFFFF) {
        return 0;  // Not enough contiguous memory
    }

    // Mark all pages as used
    for (uint32_t i = 0; i < count; i++) {
        bitmap_set(page_index + i);
        used_pages++;
    }

    return page_index * PAGE_SIZE;
}

/**
 * Free multiple contiguous pages
 */
void pmm_free_pages(uint32_t page_addr, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        pmm_free_page(page_addr + (i * PAGE_SIZE));
    }
}

/**
 * Memory statistics functions
 */
uint32_t pmm_get_total_memory(void) {
    return MEMORY_SIZE;
}

uint32_t pmm_get_used_memory(void) {
    return used_pages * PAGE_SIZE;
}

uint32_t pmm_get_free_memory(void) {
    return (TOTAL_PAGES - used_pages) * PAGE_SIZE;
}

uint32_t pmm_get_total_pages(void) {
    return TOTAL_PAGES;
}

uint32_t pmm_get_used_pages(void) {
    return used_pages;
}

uint32_t pmm_get_free_pages(void) {
    return TOTAL_PAGES - used_pages;
}
