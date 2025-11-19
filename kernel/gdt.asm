[BITS 32]

; Load GDT
; Takes GDT pointer address as parameter
global gdt_flush
gdt_flush:
    mov eax, [esp + 4]  ; Get GDT pointer from stack
    lgdt [eax]          ; Load GDT

    ; Reload segment registers
    mov ax, 0x10        ; Kernel data segment (entry 2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS (code segment)
    jmp 0x08:.flush     ; Kernel code segment (entry 1)
.flush:
    ret

; Load TSS
global tss_flush
tss_flush:
    mov ax, 0x28 | 0x03 ; TSS segment (entry 5) with RPL=3
    ltr ax              ; Load Task Register
    ret
