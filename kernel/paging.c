// Paging (Virtual Memory) implementation
#include "../include/paging.h"
#include "../include/pmm.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/string.h"
#include "../include/process.h"

// Kernel page directory (aligned to 4KB)
static page_directory_t kernel_directory __attribute__((aligned(4096)));

// Early page tables for identity mapping (16 tables for 16MB)
static page_table_t early_page_tables[16] __attribute__((aligned(4096)));

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

    // Clear early page tables
    memset(early_page_tables, 0, sizeof(early_page_tables));

    // Page table counter
    int pt_count = 0;

    for (uint32_t i = 0; i < 16 * 1024 * 1024; i += PAGE_SIZE) {
        // We need to manually create page tables since paging_map_page
        // uses pmm_alloc_page which won't work before paging is enabled

        uint32_t pd_index = get_page_dir_index(i);
        uint32_t pt_index = get_page_table_index(i);

        if (!(kernel_directory.entries[pd_index] & PAGE_PRESENT)) {
            // Use pre-allocated page table
            page_table_t *pt = &early_page_tables[pt_count++];

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
 * Error code format:
 *   Bit 0 (P)    - 0: Page not present, 1: Protection violation
 *   Bit 1 (W/R)  - 0: Read access, 1: Write access
 *   Bit 2 (U/S)  - 0: Kernel mode, 1: User mode
 *   Bit 3 (RSVD) - 1: Reserved bit violation
 *   Bit 4 (I/D)  - 1: Instruction fetch
 */
void page_fault_handler(uint32_t error_code) {
    // Read CR2 to get faulting address
    uint32_t faulting_address;
    __asm__ __volatile__("mov %%cr2, %0" : "=r"(faulting_address));

    // Parse error code
    int present = error_code & 0x1;           // Page present?
    int write = (error_code & 0x2) >> 1;      // Write access?
    int user = (error_code & 0x4) >> 2;       // User mode?
    int reserved = (error_code & 0x8) >> 3;   // Reserved bit?
    int instr_fetch = (error_code & 0x10) >> 4; // Instruction fetch?

    // Try demand paging for user mode processes
    if (!present && !reserved) {
        process_t *proc = process_current();

        // Check if fault is in valid user space region
        // Stack region: 0x80000000 - 0x10000 (stack grows down from 2GB)
        // Heap/Data region: 0x40100000 - 0x80000000 (after code)
        uint32_t stack_limit = USER_STACK_BASE - (1024 * 1024);  // 1MB stack max
        int is_stack = (faulting_address >= stack_limit && faulting_address < USER_STACK_BASE);
        int is_heap = (faulting_address >= (USER_CODE_BASE + (256 * 1024)) &&
                       faulting_address < USER_STACK_BASE);

        if (proc && proc->is_user_mode && (is_stack || is_heap)) {
            // Allocate new page for demand paging
            uint32_t page_physical = pmm_alloc_page();
            if (page_physical) {
                // Align fault address to page boundary
                uint32_t page_addr = faulting_address & 0xFFFFF000;

                // Map page to user process
                paging_map_user_code(proc->page_directory, page_addr,
                                    page_physical, PAGE_SIZE);

                kprintf_color(MAKE_COLOR(COLOR_GREEN, COLOR_BLACK),
                            "[DEMAND PAGING] Allocated page at 0x%x (phys: 0x%x)\n",
                            page_addr, page_physical);

                // Return and retry the faulting instruction
                return;
            }
        }
    }

    // Display page fault information if demand paging didn't handle it
    screen_write_color("\n\n=== PAGE FAULT ===\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));

    kprintf_color(MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK), "Fault Address: 0x%x\n", faulting_address);
    kprintf("Error Code:    0x%x\n\n", error_code);

    // Fault type
    kprintf_color(MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), "Fault Details:\n");

    if (present) {
        kprintf("  - Page protection violation\n");
    } else {
        kprintf("  - Page not present\n");
    }

    if (write) {
        kprintf("  - Write access\n");
    } else {
        if (instr_fetch) {
            kprintf("  - Instruction fetch\n");
        } else {
            kprintf("  - Read access\n");
        }
    }

    if (user) {
        kprintf("  - User mode (Ring 3)\n");
    } else {
        kprintf("  - Kernel mode (Ring 0)\n");
    }

    if (reserved) {
        kprintf("  - Reserved bit violation\n");
    }

    // Show page directory/table info
    uint32_t pd_index = faulting_address >> 22;
    uint32_t pt_index = (faulting_address >> 12) & 0x3FF;
    kprintf("\nPage Directory Index: %d\n", pd_index);
    kprintf("Page Table Index:     %d\n", pt_index);

    // Show process info if available
    process_t *proc = process_current();
    if (proc) {
        kprintf_color(MAKE_COLOR(COLOR_MAGENTA, COLOR_BLACK), "\nCurrent Process:\n");
        kprintf("  PID:  %d\n", proc->pid);
        kprintf("  Name: %s\n", proc->name);
        kprintf("  Mode: %s\n", proc->is_user_mode ? "User (Ring 3)" : "Kernel (Ring 0)");
    }

    kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK), "\nSystem halted.\n");

    // Halt the system
    __asm__ __volatile__("cli; hlt");
}

/**
 * Create new page directory for user process
 * Maps kernel space (higher half) and leaves user space empty
 */
page_directory_t *paging_create_user_directory(void) {
    // Allocate page directory
    page_directory_t *dir = (page_directory_t *)pmm_alloc_page();
    if (!dir) {
        return NULL;
    }

    // Clear directory
    memset(dir, 0, sizeof(page_directory_t));

    // Copy kernel mappings (top 256 entries = 1GB for kernel @ 3GB-4GB)
    // For now, we'll use identity mapping for kernel (first 16MB)
    // This maps kernel code/data so process can make syscalls
    for (int i = 0; i < 4; i++) {  // First 4 entries = 16MB
        dir->entries[i] = kernel_directory.entries[i];
    }

    return dir;
}

/**
 * Clone page directory (copy-on-write can be added later)
 * For now, this creates a new directory with kernel mappings
 */
page_directory_t *paging_clone_directory(page_directory_t *src) {
    (void)src;  // Not fully implemented yet
    return paging_create_user_directory();
}

/**
 * Free page directory and all its page tables
 */
void paging_free_directory(page_directory_t *dir) {
    if (!dir) return;

    // Free user space page tables (skip kernel mappings)
    for (int i = 4; i < PAGE_DIRECTORY_ENTRIES; i++) {
        if (dir->entries[i] & PAGE_PRESENT) {
            page_table_t *pt = (page_table_t *)(dir->entries[i] & 0xFFFFF000);

            // Free all pages in this page table
            for (int j = 0; j < PAGE_TABLE_ENTRIES; j++) {
                if (pt->entries[j] & PAGE_PRESENT) {
                    uint32_t physical = pt->entries[j] & 0xFFFFF000;
                    pmm_free_page(physical);
                }
            }

            // Free page table itself
            pmm_free_page((uint32_t)pt);
        }
    }

    // Free directory
    pmm_free_page((uint32_t)dir);
}

/**
 * Map user code/data to process address space
 * virtual_addr: where to map in virtual space (e.g., 0x40000000)
 * physical_addr: where code/data is in physical memory
 * size: size in bytes
 */
void paging_map_user_code(page_directory_t *dir, uint32_t virtual_addr,
                          uint32_t physical_addr, uint32_t size) {
    // Save current directory
    page_directory_t *old_dir = current_directory;

    // Switch to target directory temporarily
    current_directory = dir;

    // Map pages
    uint32_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (uint32_t i = 0; i < num_pages; i++) {
        uint32_t virt = virtual_addr + (i * PAGE_SIZE);
        uint32_t phys = physical_addr + (i * PAGE_SIZE);

        // Map with user mode flags
        paging_map_page(virt, phys, PAGE_USER_MODE);
    }

    // Restore old directory
    current_directory = old_dir;
}
