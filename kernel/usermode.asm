[BITS 32]

; Enter user mode (Ring 3)
; Parameters:
;   entry_point: User code entry point
;   user_stack: User mode stack pointer
; This function sets up Ring 3 segments and jumps to user code

global enter_usermode
enter_usermode:
    ; Disable interrupts while we set up
    cli

    ; Get parameters from kernel stack
    mov eax, [esp + 4]  ; entry_point
    mov ebx, [esp + 8]  ; user_stack

    ; IMPORTANT: We must build the IRET frame while still in kernel mode
    ; IRET frame (from bottom to top of stack):
    ; SS, ESP, EFLAGS, CS, EIP

    ; Push user data segment selector for SS (0x23)
    push dword 0x23

    ; Push user stack pointer
    push ebx

    ; Push EFLAGS with interrupt flag set
    pushf
    pop ecx
    or ecx, 0x200       ; Set IF (interrupt flag) bit 9
    push ecx

    ; Push user code segment selector (0x1B)
    push dword 0x1B

    ; Push entry point (EIP)
    push eax

    ; Now set up data segments for user mode
    ; (This is safe now because IRET frame is on kernel stack)
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Use IRET to switch to Ring 3
    ; This will pop: EIP, CS, EFLAGS, ESP, SS
    ; And automatically switch privilege level
    iret
