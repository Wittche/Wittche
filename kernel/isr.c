// Interrupt Service Routines
#include "../include/types.h"
#include "../include/screen.h"
#include "../include/keyboard.h"
#include "../include/timer.h"

// Registers struct passed from assembly
struct registers {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

// Exception messages
static char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Send EOI to PIC
extern void pic_send_eoi(uint8_t irq);

// ISR handler
void isr_handler(struct registers *regs) {
    if (regs->int_no < 32) {
        screen_write("\n");
        screen_write_color("[EXCEPTION] ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write_color(exception_messages[regs->int_no], MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write(" Exception (");
        screen_write_hex(regs->int_no);
        screen_write(")\n");

        screen_write_color("Error Code: ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write_hex(regs->err_code);
        screen_write("\n");

        screen_write_color("EIP: ", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write_hex(regs->eip);
        screen_write("\n");

        screen_write_color("\nSYSTEM HALTED!\n", MAKE_COLOR(COLOR_WHITE, COLOR_RED));

        // Hang the system
        for (;;) {
            __asm__ __volatile__("hlt");
        }
    }
}

// IRQ handler
void irq_handler(struct registers *regs) {
    // Calculate IRQ number
    uint8_t irq = regs->int_no - 32;

    // Handle specific IRQs
    switch (irq) {
        case 0:  // Timer
            timer_handler();
            break;

        case 1:  // Keyboard
            keyboard_handler();
            break;

        default:
            // Unknown IRQ
            break;
    }

    // Send End of Interrupt signal
    pic_send_eoi(irq);
}
