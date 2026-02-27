; PawNix Kernel Entry Point
; Bare metal x86, no bootloader

[org 0x7c00]      ; BIOS loads us here
[bits 16]         ; Real mode initially

start:
    ; Clear interrupts while we set up
    cli
    
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00    ; Stack grows down from here

    ; Print "P" to screen (test we're alive)
    mov ah, 0x0e       ; BIOS teletype
    mov al, 'P'
    int 0x10

    ; Load kernel into memory
    ; For now we're just in the 512 byte boot sector
    ; We need to load the REAL kernel from disk
    
    ; Switch to protected mode
    lgdt [gdt_desc]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax       ; Set PE bit
    
    ; Far jump to flush pipeline
    jmp 0x08:protected_mode

[bits 32]
protected_mode:
    ; Set up segment registers for protected mode
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack in higher memory
    mov esp, 0x90000
    
    ; Jump to kernel (we'll load it at 0x100000)
    ; For now, halt
    mov byte [0xb8000], 'K'  ; Print 'K' to VGA memory
    hlt
    jmp $

; Global Descriptor Table
gdt:
    null_desc: times 8 db 0
    code_desc: dw 0xffff    ; limit low
               dw 0          ; base low
               db 0          ; base middle
               db 10011010b  ; access (exec/read)
               db 11001111b  ; granularity (4k pages, 32-bit)
               db 0          ; base high
    data_desc: dw 0xffff    ; limit low
               dw 0          ; base low
               db 0          ; base middle
               db 10010010b  ; access (read/write)
               db 11001111b  ; granularity
               db 0          ; base high
gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt

; Fill to 510 bytes and add boot signature
times 510-($-$$) db 0
dw 0xaa55