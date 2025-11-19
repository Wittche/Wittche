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
    ; PCB structure: pid(4) + name(32) + state(4) = 40 bytes before cpu_state
    ; cpu_state_t: eax(4), ebx(4), ecx(4), edx(4), esi(4), edi(4), esp(4), ebp(4),
    ;              eip(4), eflags(4), cr3(4), cs(2), ds(2), es(2), fs(2), gs(2), ss(2)
    ; Offsets from process base:
    ; eax=40, ebx=44, ecx=48, edx=52, esi=56, edi=60, esp=64, ebp=68,
    ; eip=72, eflags=76, cr3=80, cs=84, ds=86, es=88, fs=90, gs=92, ss=94

    ; Save general purpose registers
    mov [eax + 44], ebx
    mov [eax + 48], ecx
    mov [eax + 56], esi
    mov [eax + 60], edi

    ; Save stack pointer (before this function call)
    mov ecx, esp
    add ecx, 4          ; Account for return address
    mov [eax + 64], ecx

    ; Save base pointer
    mov [eax + 68], ebp

    ; Save return address as EIP
    mov ecx, [esp]      ; Return address
    mov [eax + 72], ecx

    ; Save flags
    pushfd
    pop ecx
    mov [eax + 76], ecx

    ; Save segment registers
    mov cx, ds
    mov [eax + 86], cx  ; ds
    mov cx, es
    mov [eax + 88], cx  ; es
    mov cx, fs
    mov [eax + 90], cx  ; fs
    mov cx, gs
    mov [eax + 92], cx  ; gs
    mov cx, ss
    mov [eax + 94], cx  ; ss

.load_new:
    ; Load new process state
    test edx, edx
    jz .done

    ; Load segment registers
    mov cx, [edx + 86]  ; ds
    mov ds, cx
    mov cx, [edx + 88]  ; es
    mov es, cx
    mov cx, [edx + 90]  ; fs
    mov fs, cx
    mov cx, [edx + 92]  ; gs
    mov gs, cx
    mov cx, [edx + 94]  ; ss
    mov ss, cx

    ; Load stack pointer and base pointer
    mov esp, [edx + 64]
    mov ebp, [edx + 68]

    ; Load flags
    mov ecx, [edx + 76]
    push ecx
    popfd

    ; Load general purpose registers
    mov ebx, [edx + 44]
    mov esi, [edx + 56]
    mov edi, [edx + 60]
    mov ecx, [edx + 48]

    ; Load EIP (return address)
    mov eax, [edx + 72]
    push eax

    ; Load EDX last
    mov edx, [edx + 52]

.done:
    ret
