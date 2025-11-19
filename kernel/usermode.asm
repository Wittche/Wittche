[BITS 32]

; Enter user mode (Ring 3)
; Parameters:
;   entry_point: User code entry point
;   user_stack: User mode stack pointer
; This function sets up Ring 3 segments and jumps to user code

global enter_usermode
enter_usermode:
    ; Get parameters
    mov eax, [esp + 4]  ; entry_point
    mov ebx, [esp + 8]  ; user_stack

    ; Disable interrupts while we set up
    cli

    ; Set up user data segment (0x20 | 0x03 = 0x23)
    ; 0x20 is user data segment, 0x03 is RPL=3 (Ring 3)
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push user data segment selector for SS
    push 0x23

    ; Push user stack pointer
    push ebx

    ; Push EFLAGS with interrupt flag set
    pushf
    pop ecx
    or ecx, 0x200       ; Set IF (interrupt flag)
    push ecx

    ; Push user code segment selector (0x18 | 0x03 = 0x1B)
    ; 0x18 is user code segment, 0x03 is RPL=3
    push 0x1B

    ; Push entry point
    push eax

    ; Enable interrupts
    sti

    ; Far return to user mode
    ; This pops: EIP, CS, EFLAGS, ESP, SS
    ; Effectively switching to Ring 3
    iret
