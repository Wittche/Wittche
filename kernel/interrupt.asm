; Interrupt handlers
[BITS 32]

; External C functions
extern isr_handler
extern irq_handler

; Common ISR stub
isr_common_stub:
    ; Save all registers
    pusha
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp            ; Pass stack pointer (contains all saved registers)
    call isr_handler
    add esp, 4          ; Clean up pushed parameter

    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    popa

    ; Clean up error code and interrupt number
    add esp, 8

    ; Return from interrupt
    iret

; Common IRQ stub
irq_common_stub:
    ; Save all registers
    pusha
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler
    push esp            ; Pass stack pointer
    call irq_handler
    add esp, 4          ; Clean up

    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore registers
    popa

    ; Clean up error code and IRQ number
    add esp, 8

    ; Return from interrupt
    iret

; Macro for ISRs that don't push error code
%macro ISR_NO_ERROR 1
    global isr%1
    isr%1:
        push 0          ; Push dummy error code
        push %1         ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for ISRs that push error code
%macro ISR_ERROR 1
    global isr%1
    isr%1:
        push %1         ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for IRQs
%macro IRQ 2
    global irq%1
    irq%1:
        push 0          ; Push dummy error code
        push %2         ; Push IRQ number
        jmp irq_common_stub
%endmacro

; CPU Exception ISRs (0-31)
ISR_NO_ERROR 0      ; Divide by zero
ISR_NO_ERROR 1      ; Debug
ISR_NO_ERROR 2      ; Non-maskable interrupt
ISR_NO_ERROR 3      ; Breakpoint
ISR_NO_ERROR 4      ; Overflow
ISR_NO_ERROR 5      ; Bound range exceeded
ISR_NO_ERROR 6      ; Invalid opcode
ISR_NO_ERROR 7      ; Device not available
ISR_ERROR 8         ; Double fault (has error code)
ISR_NO_ERROR 9      ; Coprocessor segment overrun
ISR_ERROR 10        ; Invalid TSS (has error code)
ISR_ERROR 11        ; Segment not present (has error code)
ISR_ERROR 12        ; Stack-segment fault (has error code)
ISR_ERROR 13        ; General protection fault (has error code)
ISR_ERROR 14        ; Page fault (has error code)
ISR_NO_ERROR 15     ; Reserved
ISR_NO_ERROR 16     ; x87 floating-point exception
ISR_ERROR 17        ; Alignment check (has error code)
ISR_NO_ERROR 18     ; Machine check
ISR_NO_ERROR 19     ; SIMD floating-point exception
ISR_NO_ERROR 20     ; Virtualization exception
ISR_NO_ERROR 21     ; Reserved
ISR_NO_ERROR 22     ; Reserved
ISR_NO_ERROR 23     ; Reserved
ISR_NO_ERROR 24     ; Reserved
ISR_NO_ERROR 25     ; Reserved
ISR_NO_ERROR 26     ; Reserved
ISR_NO_ERROR 27     ; Reserved
ISR_NO_ERROR 28     ; Reserved
ISR_NO_ERROR 29     ; Reserved
ISR_ERROR 30        ; Security exception (has error code)
ISR_NO_ERROR 31     ; Reserved

; Hardware IRQs (32-47)
IRQ 0, 32           ; Timer
IRQ 1, 33           ; Keyboard
IRQ 2, 34           ; Cascade
IRQ 3, 35           ; COM2
IRQ 4, 36           ; COM1
IRQ 5, 37           ; LPT2
IRQ 6, 38           ; Floppy disk
IRQ 7, 39           ; LPT1
IRQ 8, 40           ; CMOS real-time clock
IRQ 9, 41           ; Free
IRQ 10, 42          ; Free
IRQ 11, 43          ; Free
IRQ 12, 44          ; PS2 Mouse
IRQ 13, 45          ; FPU
IRQ 14, 46          ; Primary ATA
IRQ 15, 47          ; Secondary ATA

; Load IDT
global idt_load
idt_load:
    mov eax, [esp + 4]  ; Get pointer to IDT pointer
    lidt [eax]          ; Load IDT
    ret
