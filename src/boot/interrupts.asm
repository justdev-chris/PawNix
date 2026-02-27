; PawNix Interrupt Service Routines
; Assembly stubs for kernel.c

section .text
global gdt_flush
global idt_load
global isr0_asm, isr1_asm, isr2_asm, isr3_asm, isr4_asm
global isr5_asm, isr6_asm, isr7_asm, isr8_asm, isr9_asm
global isr10_asm, isr11_asm, isr12_asm, isr13_asm, isr14_asm
global isr15_asm, isr16_asm, isr17_asm, isr18_asm, isr19_asm
global isr20_asm, isr21_asm, isr22_asm, isr23_asm, isr24_asm
global isr25_asm, isr26_asm, isr27_asm, isr28_asm, isr29_asm
global isr30_asm, isr31_asm
global irq0_asm, irq1_asm, irq2_asm, irq3_asm, irq4_asm
global irq5_asm, irq6_asm, irq7_asm, irq8_asm, irq9_asm
global irq10_asm, irq11_asm, irq12_asm, irq13_asm, irq14_asm
global irq15_asm

; External C handlers
extern isr0, isr1, isr2, isr3, isr4
extern isr5, isr6, isr7, isr8, isr9
extern isr10, isr11, isr12, isr13, isr14
extern isr15, isr16, isr17, isr18, isr19
extern isr20, isr21, isr22, isr23, isr24
extern isr25, isr26, isr27, isr28, isr29
extern isr30, isr31
extern irq0, irq1, irq2, irq3, irq4
extern irq5, irq6, irq7, irq8, irq9
extern irq10, irq11, irq12, irq13, irq14
extern irq15

; GDT flush
gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

; IDT load
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret

; Common ISR stub
%macro ISR_NOERRCODE 1
isr%1_asm:
    cli
    push 0          ; dummy error code
    push %1         ; interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
isr%1_asm:
    cli
    push %1         ; interrupt number
    jmp isr_common
%endmacro

; Common IRQ stub
%macro IRQ 2
irq%1_asm:
    cli
    push 0          ; dummy error code
    push %2         ; interrupt number
    jmp irq_common
%endmacro

; ISRs 0-31
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; IRQs 32-47
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

; Common ISR handler
isr_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, [esp+8]    ; Get interrupt number
    call [isr_table + eax*4]
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8          ; Clean up error code and int number
    iret

; Common IRQ handler
irq_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, [esp+8]    ; Get interrupt number
    sub eax, 32         ; Adjust for IRQ offset
    call [irq_table + eax*4]
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret

; ISR function table
section .data
isr_table:
    dd isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
    dd isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
    dd isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
    dd isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

irq_table:
    dd irq0, irq1, irq2, irq3, irq4, irq5, irq6, irq7
    dd irq8, irq9, irq10, irq11, irq12, irq13, irq14, irq15