; Simple bootloader for x86
; This bootloader loads the kernel into memory and jumps to it

[BITS 16]           ; We start in 16-bit real mode
[ORG 0x7C00]        ; BIOS loads bootloader at 0x7C00

start:
    ; Set up segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00      ; Stack grows downward from bootloader

    ; Print loading message
    mov si, msg_loading
    call print_string

    ; Load kernel from disk
    ; We'll load sectors 2-40 (kernel) to 0x1000:0x0000
    mov ah, 0x02        ; BIOS read sector function
    mov al, 40          ; Number of sectors to read (20KB)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2 (sector 1 is boot sector)
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; Hard drive (use 0x00 for floppy)
    mov bx, 0x1000      ; ES:BX = 0x1000:0x0000
    mov es, bx
    xor bx, bx
    int 0x13            ; Call BIOS

    jc disk_error       ; Jump if carry flag set (error)

    ; Print success message
    mov si, msg_success
    call print_string

    ; Switch to protected mode
    cli                 ; Disable interrupts
    lgdt [gdt_descriptor]   ; Load GDT

    mov eax, cr0
    or eax, 1           ; Set PE (Protection Enable) bit
    mov cr0, eax

    ; Far jump to 32-bit code segment
    jmp 0x08:protected_mode

disk_error:
    mov si, msg_error
    call print_string
    jmp $               ; Hang

; Print string function (16-bit real mode)
print_string:
    pusha
    mov ah, 0x0E        ; BIOS teletype function
.loop:
    lodsb               ; Load byte from SI into AL
    test al, al         ; Check if zero
    jz .done
    int 0x10            ; Print character
    jmp .loop
.done:
    popa
    ret

; Messages
msg_loading: db "Loading kernel...", 13, 10, 0
msg_success: db "Success! Entering protected mode...", 13, 10, 0
msg_error: db "Disk read error!", 13, 10, 0

; GDT (Global Descriptor Table)
gdt_start:
    ; Null descriptor
    dq 0x0

    ; Code segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10011010b    ; Access byte: present, ring 0, code, executable, readable
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

    ; Data segment descriptor
    dw 0xFFFF       ; Limit (bits 0-15)
    dw 0x0          ; Base (bits 0-15)
    db 0x0          ; Base (bits 16-23)
    db 10010010b    ; Access byte: present, ring 0, data, writable
    db 11001111b    ; Flags + Limit (bits 16-19)
    db 0x0          ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

[BITS 32]
protected_mode:
    ; Set up segment registers for protected mode
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000    ; Set up stack

    ; Clear VGA text buffer completely (0xB8000 - 0xB8FA0)
    ; This removes all BIOS messages and ensures clean start
    mov edi, 0xB8000    ; VGA text buffer start
    mov ecx, 2000       ; 80x25 = 2000 characters
    mov ax, 0x0720      ; Light grey on black space
.clear_loop:
    mov [edi], ax       ; Write space with color
    add edi, 2          ; Move to next character (2 bytes per char)
    loop .clear_loop    ; Repeat until ecx = 0

    ; Reset cursor to position 0
    mov dx, 0x3D4       ; VGA cursor control register
    mov al, 0x0F        ; Cursor location low byte
    out dx, al
    mov dx, 0x3D5
    mov al, 0x00        ; Position 0
    out dx, al

    mov dx, 0x3D4
    mov al, 0x0E        ; Cursor location high byte
    out dx, al
    mov dx, 0x3D5
    mov al, 0x00        ; Position 0
    out dx, al

    ; Jump to kernel (VGA is now clean)
    call 0x10000        ; Call kernel (will return here if kernel returns)

    ; If we get here, kernel returned - hang
    jmp $

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55           ; Boot signature
