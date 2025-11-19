// GDT (Global Descriptor Table) implementation
// Manages memory segmentation and privilege levels

#include "../include/gdt.h"
#include "../include/string.h"
#include "../include/kprintf.h"

// GDT with 6 entries
static gdt_entry_t gdt_entries[6];
static gdt_ptr_t gdt_ptr;
static tss_t tss_entry;

// External assembly function to load GDT
extern void gdt_flush(uint32_t gdt_ptr_addr);

// External assembly function to load TSS
extern void tss_flush(void);

/**
 * Set a GDT entry
 */
static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    // Base address
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    // Limit
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = ((limit >> 16) & 0x0F);

    // Granularity
    gdt_entries[num].granularity |= (gran & 0xF0);

    // Access flags
    gdt_entries[num].access = access;
}

/**
 * Initialize GDT
 */
void gdt_init(void) {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // Entry 0: Null descriptor
    gdt_set_gate(0, 0, 0, 0, 0);

    // Entry 1: Kernel code segment (Ring 0)
    // Base = 0, Limit = 4GB
    // Access = 0x9A: Present, Ring 0, Code, Executable, Readable
    // Granularity = 0xCF: 4KB granularity, 32-bit
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // Entry 2: Kernel data segment (Ring 0)
    // Base = 0, Limit = 4GB
    // Access = 0x92: Present, Ring 0, Data, Writable
    // Granularity = 0xCF: 4KB granularity, 32-bit
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Entry 3: User code segment (Ring 3)
    // Base = 0, Limit = 4GB
    // Access = 0xFA: Present, Ring 3, Code, Executable, Readable
    // Granularity = 0xCF: 4KB granularity, 32-bit
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    // Entry 4: User data segment (Ring 3)
    // Base = 0, Limit = 4GB
    // Access = 0xF2: Present, Ring 3, Data, Writable
    // Granularity = 0xCF: 4KB granularity, 32-bit
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    // Entry 5: TSS (Task State Segment)
    // Access = 0x89: Present, Ring 0, TSS, Available
    // Granularity = 0x40: Byte granularity
    uint32_t tss_base = (uint32_t)&tss_entry;
    uint32_t tss_limit = sizeof(tss_t) - 1;
    gdt_set_gate(5, tss_base, tss_limit, 0x89, 0x40);

    // Initialize TSS
    memset(&tss_entry, 0, sizeof(tss_t));

    // Set TSS segments
    tss_entry.ss0 = KERNEL_DATA_SEGMENT;  // Kernel data segment
    tss_entry.esp0 = 0x0;  // Will be set during context switches
    tss_entry.cs = KERNEL_CODE_SEGMENT | 0x03;  // Kernel code with RPL=3
    tss_entry.ss = KERNEL_DATA_SEGMENT | 0x03;  // Kernel data with RPL=3
    tss_entry.ds = KERNEL_DATA_SEGMENT | 0x03;
    tss_entry.es = KERNEL_DATA_SEGMENT | 0x03;
    tss_entry.fs = KERNEL_DATA_SEGMENT | 0x03;
    tss_entry.gs = KERNEL_DATA_SEGMENT | 0x03;

    // Load GDT and TSS
    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();

    kprintf("[GDT] Global Descriptor Table initialized\n");
    kprintf("[GDT] Segments: Kernel (Ring 0), User (Ring 3), TSS\n");
}

/**
 * Set TSS kernel stack
 * Called during context switch to set the kernel stack for the current process
 */
void tss_set_kernel_stack(uint32_t stack) {
    tss_entry.esp0 = stack;
}
