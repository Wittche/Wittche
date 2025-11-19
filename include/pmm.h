// Physical Memory Manager header
#ifndef PMM_H
#define PMM_H

#include "types.h"

// Memory constants
#define PAGE_SIZE 4096              // 4KB pages
#define MEMORY_SIZE (16 * 1024 * 1024)  // 16MB total memory
#define TOTAL_PAGES (MEMORY_SIZE / PAGE_SIZE)  // 4096 pages

// Kernel occupies first 2MB (0x000000 - 0x200000)
#define KERNEL_END 0x200000
#define USABLE_MEMORY_START KERNEL_END

// Memory region types
#define MEMORY_REGION_FREE 0
#define MEMORY_REGION_RESERVED 1
#define MEMORY_REGION_KERNEL 2

/**
 * Physical Memory Manager functions
 */

// Initialize physical memory manager
void pmm_init(void);

// Allocate a physical page (4KB)
// Returns physical address or 0 if failed
uint32_t pmm_alloc_page(void);

// Free a physical page
void pmm_free_page(uint32_t page_addr);

// Allocate multiple contiguous pages
uint32_t pmm_alloc_pages(uint32_t count);

// Free multiple contiguous pages
void pmm_free_pages(uint32_t page_addr, uint32_t count);

// Get memory statistics
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_used_memory(void);
uint32_t pmm_get_free_memory(void);
uint32_t pmm_get_total_pages(void);
uint32_t pmm_get_used_pages(void);
uint32_t pmm_get_free_pages(void);

#endif // PMM_H
