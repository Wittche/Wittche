// Paging (Virtual Memory) header
#ifndef PAGING_H
#define PAGING_H

#include "types.h"

// Page directory/table entry flags
#define PAGE_PRESENT    0x01    // Page is present in memory
#define PAGE_WRITE      0x02    // Page is writable
#define PAGE_USER       0x04    // Page is accessible from user mode
#define PAGE_WRITETHROUGH 0x08  // Write-through caching
#define PAGE_NOCACHE    0x10    // Disable cache for this page
#define PAGE_ACCESSED   0x20    // Page has been accessed
#define PAGE_DIRTY      0x40    // Page has been written to (PTE only)
#define PAGE_SIZE_4MB   0x80    // 4MB pages (only in PDE with PSE)
#define PAGE_GLOBAL     0x100   // Global page (not flushed from TLB)

// Default page flags
#define PAGE_DEFAULT (PAGE_PRESENT | PAGE_WRITE)
#define PAGE_KERNEL (PAGE_PRESENT | PAGE_WRITE)
#define PAGE_USER_MODE (PAGE_PRESENT | PAGE_WRITE | PAGE_USER)

// Page directory and table sizes
#define PAGE_DIRECTORY_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

/**
 * Page directory entry (points to page table)
 */
typedef uint32_t page_directory_entry_t;

/**
 * Page table entry (points to physical page)
 */
typedef uint32_t page_table_entry_t;

/**
 * Page directory structure
 */
typedef struct {
    page_directory_entry_t entries[PAGE_DIRECTORY_ENTRIES];
} __attribute__((aligned(4096))) page_directory_t;

/**
 * Page table structure
 */
typedef struct {
    page_table_entry_t entries[PAGE_TABLE_ENTRIES];
} __attribute__((aligned(4096))) page_table_t;

/**
 * Paging functions
 */

// Initialize paging system
void paging_init(void);

// Enable paging
void paging_enable(void);

// Disable paging
void paging_disable(void);

// Map a virtual page to a physical page
void paging_map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);

// Unmap a virtual page
void paging_unmap_page(uint32_t virtual_addr);

// Get physical address for virtual address
uint32_t paging_get_physical_address(uint32_t virtual_addr);

// Flush TLB for a specific page
void paging_flush_tlb_entry(uint32_t virtual_addr);

// Flush entire TLB
void paging_flush_tlb(void);

// Get current page directory
page_directory_t *paging_get_directory(void);

// Switch page directory
void paging_switch_directory(page_directory_t *dir);

// Page fault handler
void page_fault_handler(void);

#endif // PAGING_H
