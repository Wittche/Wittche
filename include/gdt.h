#ifndef GDT_H
#define GDT_H

#include "types.h"

// GDT Entry structure
typedef struct {
    uint16_t limit_low;      // Lower 16 bits of limit
    uint16_t base_low;       // Lower 16 bits of base
    uint8_t base_middle;     // Next 8 bits of base
    uint8_t access;          // Access flags
    uint8_t granularity;     // Granularity and limit high
    uint8_t base_high;       // High 8 bits of base
} __attribute__((packed)) gdt_entry_t;

// GDT Pointer structure
typedef struct {
    uint16_t limit;          // Size of GDT - 1
    uint32_t base;           // Address of first GDT entry
} __attribute__((packed)) gdt_ptr_t;

// TSS (Task State Segment) structure
typedef struct {
    uint32_t prev_tss;       // Previous TSS (unused)
    uint32_t esp0;           // Stack pointer for kernel mode
    uint32_t ss0;            // Stack segment for kernel mode
    uint32_t esp1;           // Unused
    uint32_t ss1;            // Unused
    uint32_t esp2;           // Unused
    uint32_t ss2;            // Unused
    uint32_t cr3;            // Page directory base
    uint32_t eip;            // Instruction pointer
    uint32_t eflags;         // Flags
    uint32_t eax;            // General purpose registers
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;             // Segment selectors
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;            // LDT selector (unused)
    uint16_t trap;           // Trap flag
    uint16_t iomap_base;     // I/O map base address
} __attribute__((packed)) tss_t;

// Segment selectors
#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10
#define USER_CODE_SEGMENT   0x18
#define USER_DATA_SEGMENT   0x20
#define TSS_SEGMENT         0x28

// Initialize GDT
void gdt_init(void);

// Set TSS kernel stack (called during context switch)
void tss_set_kernel_stack(uint32_t stack);

#endif // GDT_H
