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
    ; BIOS can only read ~63 sectors at once, so we do multiple reads
    ; CHS: Sectors numbered 1-63, Heads 0-1+, Cylinders 0+

    ; Read 1: Cyl 0, Head 0, Sectors 2-63 (62 sectors) to 0x1000:0x0000
    mov ah, 0x02        ; BIOS read sector function
    mov al, 62          ; Read 62 sectors (from sector 2 to 63)
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start from sector 2 (sector 1 is boot sector)
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; Hard drive (use 0x00 for floppy)
    mov bx, 0x1000      ; ES:BX = 0x1000:0x0000 = 0x10000 linear
    mov es, bx
    xor bx, bx
    int 0x13            ; Call BIOS
    jc disk_error       ; Jump if carry flag set (error)

    ; Read 2: Cyl 0, Head 1, Sectors 1-63 (63 sectors) to 0x17C0:0x0000
    ; Previous: 62 sectors * 512 bytes = 31744 = 0x7C00
    ; Next address: 0x10000 + 0x7C00 = 0x17C00 = segment 0x17C0
    mov ah, 0x02        ; BIOS read sector function
    mov al, 63          ; Read 63 sectors
    mov ch, 0           ; Cylinder 0
    mov cl, 1           ; Start from sector 1
    mov dh, 1           ; Head 1 (next track)
    mov dl, 0x80        ; Hard drive
    mov bx, 0x17C0      ; ES = 0x17C0
    mov es, bx
    xor bx, bx          ; BX = 0x0000
    int 0x13            ; Call BIOS
    jc disk_error       ; Jump if carry flag set (error)

    ; Read 3: Cyl 1, Head 0, Sectors 1-63 (63 sectors) to 0x1FA0:0x0000
    ; Previous: 125 sectors * 512 bytes = 64000 = 0xFA00
    ; Next address: 0x10000 + 0xFA00 = 0x1FA00 = segment 0x1FA0
    mov ah, 0x02        ; BIOS read sector function
    mov al, 63          ; Read 63 sectors
    mov ch, 1           ; Cylinder 1 (next cylinder)
    mov cl, 1           ; Start from sector 1
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; Hard drive
    mov bx, 0x1FA0      ; ES = 0x1FA0
    mov es, bx
    xor bx, bx          ; BX = 0x0000
    int 0x13            ; Call BIOS
    jc disk_error       ; Jump if carry flag set (error)

    ; Read 4: Cyl 1, Head 1, Sectors 1-63 (63 sectors) to 0x2780:0x0000
    ; Previous: 188 sectors * 512 bytes = 96256 = 0x17800
    ; Next address: 0x10000 + 0x17800 = 0x27800 = segment 0x2780
    ; This gives us total 251 sectors (~128KB) for kernel
    mov ah, 0x02        ; BIOS read sector function
    mov al, 63          ; Read 63 sectors
    mov ch, 1           ; Cylinder 1
    mov cl, 1           ; Start from sector 1
    mov dh, 1           ; Head 1 (next track)
    mov dl, 0x80        ; Hard drive
    mov bx, 0x2780      ; ES = 0x2780
    mov es, bx
    xor bx, bx          ; BX = 0x0000
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

    ; DON'T clear screen - let kernel do it
    ; This way we can see BIOS messages and debug bootloader

    ; Jump to kernel (VGA is now clean)
    jmp 0x10000         ; Jump to kernel (changed from call to jmp)

    ; If we get here, kernel returned - hang
    jmp $

; Pad to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xAA55           ; Boot signature
