; Context switching assembly code
; void process_switch(process_t *old_process, process_t *new_process);

section .text
global process_switch

process_switch:
    ; Function parameters:
    ; [esp + 4] = old_process
    ; [esp + 8] = new_process

    ; Get parameters
    mov eax, [esp + 4]  ; old_process
    mov edx, [esp + 8]  ; new_process

    ; Save old process state (if not NULL)
    test eax, eax
    jz .load_new

    ; Save registers to old process CPU state
    ; Offset in cpu_state_t structure:
    ; +36 = eax, +40 = ebx, +44 = ecx, +48 = edx
    ; +52 = esi, +56 = edi, +60 = esp, +64 = ebp
    ; +68 = eip, +72 = eflags

    ; Save general purpose registers
    mov [eax + 40], ebx
    mov [eax + 44], ecx
    mov [eax + 52], esi
    mov [eax + 56], edi

    ; Save stack pointer (before this function call)
    mov ecx, esp
    add ecx, 4          ; Account for return address
    mov [eax + 60], ecx

    ; Save base pointer
    mov [eax + 64], ebp

    ; Save return address as EIP
    mov ecx, [esp]      ; Return address
    mov [eax + 68], ecx

    ; Save flags
    pushfd
    pop ecx
    mov [eax + 72], ecx

    ; Save segment registers
    mov cx, ds
    mov [eax + 80], cx  ; ds at offset 80
    mov cx, es
    mov [eax + 82], cx  ; es at offset 82
    mov cx, fs
    mov [eax + 84], cx  ; fs at offset 84
    mov cx, gs
    mov [eax + 86], cx  ; gs at offset 86
    mov cx, ss
    mov [eax + 88], cx  ; ss at offset 88

.load_new:
    ; Load new process state
    test edx, edx
    jz .done

    ; Load segment registers
    mov cx, [edx + 80]  ; ds
    mov ds, cx
    mov cx, [edx + 82]  ; es
    mov es, cx
    mov cx, [edx + 84]  ; fs
    mov fs, cx
    mov cx, [edx + 86]  ; gs
    mov gs, cx
    mov cx, [edx + 88]  ; ss
    mov ss, cx

    ; Load stack pointer and base pointer
    mov esp, [edx + 60]
    mov ebp, [edx + 64]

    ; Load flags
    mov ecx, [edx + 72]
    push ecx
    popfd

    ; Load general purpose registers
    mov ebx, [edx + 40]
    mov esi, [edx + 52]
    mov edi, [edx + 56]
    mov ecx, [edx + 44]

    ; Load EIP (return address)
    mov eax, [edx + 68]
    push eax

    ; Load EDX last
    mov edx, [edx + 48]

.done:
    ret
