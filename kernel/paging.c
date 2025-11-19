// Paging (Virtual Memory) implementation
#include "../include/paging.h"
#include "../include/pmm.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/string.h"

// Kernel page directory (aligned to 4KB)
static page_directory_t kernel_directory __attribute__((aligned(4096)));

// Current page directory
static page_directory_t *current_directory = NULL;

/**
 * Get page directory index from virtual address
 */
static inline uint32_t get_page_dir_index(uint32_t virtual_addr) {
    return virtual_addr >> 22;  // Top 10 bits
}

/**
 * Get page table index from virtual address
 */
static inline uint32_t get_page_table_index(uint32_t virtual_addr) {
    return (virtual_addr >> 12) & 0x3FF;  // Middle 10 bits
}

/**
 * Get page offset from virtual address
 */
static inline uint32_t get_page_offset(uint32_t virtual_addr) {
    return virtual_addr & 0xFFF;  // Bottom 12 bits
}

/**
 * Create a page directory entry
 */
static inline page_directory_entry_t make_pde(uint32_t page_table_addr, uint32_t flags) {
    return (page_table_addr & 0xFFFFF000) | (flags & 0xFFF);
}

/**
 * Create a page table entry
 */
static inline page_table_entry_t make_pte(uint32_t physical_addr, uint32_t flags) {
    return (physical_addr & 0xFFFFF000) | (flags & 0xFFF);
}

/**
 * Load page directory into CR3
 */
static void load_page_directory(page_directory_t *dir) {
    uint32_t dir_physical = (uint32_t)dir;
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(dir_physical));
}

/**
 * Enable paging (set CR0 bit 31)
 */
void paging_enable(void) {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;  // Set PG bit (bit 31)
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}

/**
 * Disable paging (clear CR0 bit 31)
 */
void paging_disable(void) {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~0x80000000;  // Clear PG bit (bit 31)
    __asm__ __volatile__("mov %0, %%cr0" : : "r"(cr0));
}

/**
 * Flush TLB for specific page
 */
void paging_flush_tlb_entry(uint32_t virtual_addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

/**
 * Flush entire TLB (reload CR3)
 */
void paging_flush_tlb(void) {
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %0" : "=r"(cr3));
    __asm__ __volatile__("mov %0, %%cr3" : : "r"(cr3));
}

/**
 * Map a virtual page to physical page
 */
void paging_map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t pd_index = get_page_dir_index(virtual_addr);
    uint32_t pt_index = get_page_table_index(virtual_addr);

    // Get page directory entry
    page_directory_entry_t *pde = &current_directory->entries[pd_index];

    page_table_t *page_table;

    // If page table doesn't exist, create it
    if (!(*pde & PAGE_PRESENT)) {
        // Allocate physical page for page table
        uint32_t pt_physical = pmm_alloc_page();
        if (!pt_physical) {
            kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                         "[PAGING ERROR] Failed to allocate page table\n");
            return;
        }

        // Clear page table
        page_table = (page_table_t *)pt_physical;
        memset(page_table, 0, sizeof(page_table_t));

        // Set page directory entry
        *pde = make_pde(pt_physical, PAGE_PRESENT | PAGE_WRITE);
    } else {
        // Get existing page table
        page_table = (page_table_t *)(*pde & 0xFFFFF000);
    }

    // Set page table entry
    page_table->entries[pt_index] = make_pte(physical_addr, flags);

    // Flush TLB for this page
    paging_flush_tlb_entry(virtual_addr);
}

/**
 * Unmap a virtual page
 */
void paging_unmap_page(uint32_t virtual_addr) {
    uint32_t pd_index = get_page_dir_index(virtual_addr);
    uint32_t pt_index = get_page_table_index(virtual_addr);

    page_directory_entry_t *pde = &current_directory->entries[pd_index];

    if (!(*pde & PAGE_PRESENT)) {
        return;  // Page table doesn't exist
    }

    page_table_t *page_table = (page_table_t *)(*pde & 0xFFFFF000);

    // Clear page table entry
    page_table->entries[pt_index] = 0;

    // Flush TLB
    paging_flush_tlb_entry(virtual_addr);
}

/**
 * Get physical address for virtual address
 */
uint32_t paging_get_physical_address(uint32_t virtual_addr) {
    uint32_t pd_index = get_page_dir_index(virtual_addr);
    uint32_t pt_index = get_page_table_index(virtual_addr);
    uint32_t offset = get_page_offset(virtual_addr);

    page_directory_entry_t pde = current_directory->entries[pd_index];

    if (!(pde & PAGE_PRESENT)) {
        return 0;  // Page table not present
    }

    page_table_t *page_table = (page_table_t *)(pde & 0xFFFFF000);
    page_table_entry_t pte = page_table->entries[pt_index];

    if (!(pte & PAGE_PRESENT)) {
        return 0;  // Page not present
    }

    return (pte & 0xFFFFF000) | offset;
}

/**
 * Get current page directory
 */
page_directory_t *paging_get_directory(void) {
    return current_directory;
}

/**
 * Switch page directory
 */
void paging_switch_directory(page_directory_t *dir) {
    current_directory = dir;
    load_page_directory(dir);
    paging_flush_tlb();
}

/**
 * Initialize paging
 */
void paging_init(void) {
    screen_write_color("[PAGING] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("Initializing virtual memory...\n");

    // Clear kernel page directory
    memset(&kernel_directory, 0, sizeof(page_directory_t));

    // Identity map first 16MB (kernel + heap)
    // This maps virtual address = physical address
    kprintf("         Identity mapping first 16 MB...\n");

    for (uint32_t i = 0; i < 16 * 1024 * 1024; i += PAGE_SIZE) {
        // We need to manually create page tables since paging_map_page
        // uses pmm_alloc_page which won't work before paging is enabled

        uint32_t pd_index = get_page_dir_index(i);
        uint32_t pt_index = get_page_table_index(i);

        // Allocate page table if needed (use static allocation)
        static page_table_t early_page_tables[16] __attribute__((aligned(4096)));
        static int pt_count = 0;

        if (!(kernel_directory.entries[pd_index] & PAGE_PRESENT)) {
            // Use pre-allocated page table
            page_table_t *pt = &early_page_tables[pt_count++];
            memset(pt, 0, sizeof(page_table_t));

            kernel_directory.entries[pd_index] = make_pde((uint32_t)pt, PAGE_KERNEL);
        }

        page_table_t *page_table = (page_table_t *)(kernel_directory.entries[pd_index] & 0xFFFFF000);
        page_table->entries[pt_index] = make_pte(i, PAGE_KERNEL);
    }

    // Set current directory
    current_directory = &kernel_directory;

    // Load page directory into CR3
    load_page_directory(&kernel_directory);

    // Enable paging
    paging_enable();

    kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                 "         Paging enabled! Virtual memory active.\n");
}

/**
 * Page fault handler
 */
void page_fault_handler(void) {
    // Read CR2 to get faulting address
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));

    screen_write_color("\n[PAGE FAULT] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
    kprintf("at address 0x%x\n", faulting_address);

    // For now, just halt
    __asm__ __volatile__("cli; hlt");
}
