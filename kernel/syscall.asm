[BITS 32]

; System call interrupt handler (INT 0x80)
; System call convention:
;   EAX = syscall number
;   EBX = arg1
;   ECX = arg2
;   EDX = arg3
;   ESI = arg4
;   Return value in EAX

extern syscall_dispatch

global syscall_interrupt_handler
syscall_interrupt_handler:
    ; Save all registers
    push ebp
    push edi
    push esi
    push edx
    push ecx
    push ebx
    push eax

    ; Push arguments in reverse order (for C calling convention)
    push esi    ; arg4
    push edx    ; arg3
    push ecx    ; arg2
    push ebx    ; arg1
    push eax    ; syscall number

    ; Call C dispatcher
    call syscall_dispatch

    ; Clean up stack (5 arguments * 4 bytes = 20 bytes)
    add esp, 20

    ; Save return value
    mov [esp], eax  ; Overwrite saved EAX with return value

    ; Restore all registers (EAX will have return value)
    pop eax
    pop ebx
    pop ecx
    pop edx
    pop esi
    pop edi
    pop ebp

    ; Return from interrupt
    iret
