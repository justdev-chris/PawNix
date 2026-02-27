// PawNix/src/kernel.c
// Complete bare kernel for distro builders

// VGA stuff
#define VGA_ADDRESS 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define WHITE_ON_BLACK 0x07

// Terminal state
int terminal_row = 0;
int terminal_column = 0;
char* terminal_buffer = (char*) VGA_ADDRESS;

// GDT stuff
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

struct gdt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gp;

// IDT stuff
struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr ip;

// ISR handlers
void isr0() { /* Divide by zero */ }
void isr1() { /* Debug */ }
void isr2() { /* NMI */ }
void isr3() { /* Breakpoint */ }
void isr4() { /* Overflow */ }
void isr5() { /* Bound range */ }
void isr6() { /* Invalid opcode */ }
void isr7() { /* Device not available */ }
void isr8() { /* Double fault */ }
void isr9() { /* Coprocessor segment overrun */ }
void isr10() { /* Invalid TSS */ }
void isr11() { /* Segment not present */ }
void isr12() { /* Stack segment fault */ }
void isr13() { /* General protection fault */ }
void isr14() { /* Page fault */ }
void isr15() { /* Reserved */ }
void isr16() { /* Floating point error */ }
void isr17() { /* Alignment check */ }
void isr18() { /* Machine check */ }
void isr19() { /* SIMD floating point */ }
void isr20() { /* Virtualization */ }
void isr21() { /* Control protection */ }
void isr22() { /* Reserved */ }
void isr23() { /* Reserved */ }
void isr24() { /* Reserved */ }
void isr25() { /* Reserved */ }
void isr26() { /* Reserved */ }
void isr27() { /* Reserved */ }
void isr28() { /* Reserved */ }
void isr29() { /* Reserved */ }
void isr30() { /* Reserved */ }
void isr31() { /* Reserved */ }

// IRQ handlers
void irq0() { /* Programmable Interrupt Timer */ }
void irq1() { /* Keyboard */ }
void irq2() { /* Cascade */ }
void irq3() { /* COM2 */ }
void irq4() { /* COM1 */ }
void irq5() { /* LPT2 */ }
void irq6() { /* Floppy disk */ }
void irq7() { /* LPT1 */ }
void irq8() { /* CMOS real-time clock */ }
void irq9() { /* Free */ }
void irq10() { /* Free */ }
void irq11() { /* Free */ }
void irq12() { /* PS2 Mouse */ }
void irq13() { /* FPU */ }
void irq14() { /* Primary ATA */ }
void irq15() { /* Secondary ATA */ }

// Assembly stubs for ISRs
extern void isr0_asm();
extern void isr1_asm();
extern void isr2_asm();
extern void isr3_asm();
extern void isr4_asm();
extern void isr5_asm();
extern void isr6_asm();
extern void isr7_asm();
extern void isr8_asm();
extern void isr9_asm();
extern void isr10_asm();
extern void isr11_asm();
extern void isr12_asm();
extern void isr13_asm();
extern void isr14_asm();
extern void isr15_asm();
extern void isr16_asm();
extern void isr17_asm();
extern void isr18_asm();
extern void isr19_asm();
extern void isr20_asm();
extern void isr21_asm();
extern void isr22_asm();
extern void isr23_asm();
extern void isr24_asm();
extern void isr25_asm();
extern void isr26_asm();
extern void isr27_asm();
extern void isr28_asm();
extern void isr29_asm();
extern void isr30_asm();
extern void isr31_asm();

// Assembly stubs for IRQs
extern void irq0_asm();
extern void irq1_asm();
extern void irq2_asm();
extern void irq3_asm();
extern void irq4_asm();
extern void irq5_asm();
extern void irq6_asm();
extern void irq7_asm();
extern void irq8_asm();
extern void irq9_asm();
extern void irq10_asm();
extern void irq11_asm();
extern void irq12_asm();
extern void irq13_asm();
extern void irq14_asm();
extern void irq15_asm();

// Memory management
#define PAGE_SIZE 4096
#define MAX_PAGES 1048576  // For 4GB address space

unsigned int page_bitmap[MAX_PAGES / 32];
unsigned int memory_size = 0;

// Simple terminal functions
void terminal_initialize() {
    terminal_row = 0;
    terminal_column = 0;
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y * VGA_WIDTH + x) * 2] = ' ';
            terminal_buffer[(y * VGA_WIDTH + x) * 2 + 1] = WHITE_ON_BLACK;
        }
    }
}

void terminal_writechar(char c) {
    if (c == '\n') {
        terminal_row++;
        terminal_column = 0;
        return;
    }
    
    terminal_buffer[(terminal_row * VGA_WIDTH + terminal_column) * 2] = c;
    terminal_buffer[(terminal_row * VGA_WIDTH + terminal_column) * 2 + 1] = WHITE_ON_BLACK;
    
    terminal_column++;
    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
    }
}

void terminal_writestring(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_writechar(str[i]);
    }
}

// GDT functions
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran) {
    gdt[num].base_low = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high = (base >> 24) & 0xFF;
    gdt[num].limit_low = (limit & 0xFFFF);
    gdt[num].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[num].access = access;
}

extern void gdt_flush(unsigned int);

void gdt_install() {
    gp.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gp.base = (unsigned int)&gdt;
    
    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    
    gdt_flush((unsigned int)&gp);
}

// IDT functions
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

extern void idt_load(unsigned int);

void idt_install() {
    ip.limit = sizeof(struct idt_entry) * 256 - 1;
    ip.base = (unsigned int)&idt;
    
    // Set up ISRs
    idt_set_gate(0, (unsigned)isr0_asm, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1_asm, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2_asm, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3_asm, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4_asm, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5_asm, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6_asm, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7_asm, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr8_asm, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9_asm, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10_asm, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11_asm, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12_asm, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13_asm, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14_asm, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15_asm, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr16_asm, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17_asm, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18_asm, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19_asm, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20_asm, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21_asm, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22_asm, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23_asm, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr24_asm, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25_asm, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26_asm, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27_asm, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28_asm, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29_asm, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30_asm, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31_asm, 0x08, 0x8E);
    
    // Set up IRQs
    idt_set_gate(32, (unsigned)irq0_asm, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1_asm, 0x08, 0x8E);
    idt_set_gate(34, (unsigned)irq2_asm, 0x08, 0x8E);
    idt_set_gate(35, (unsigned)irq3_asm, 0x08, 0x8E);
    idt_set_gate(36, (unsigned)irq4_asm, 0x08, 0x8E);
    idt_set_gate(37, (unsigned)irq5_asm, 0x08, 0x8E);
    idt_set_gate(38, (unsigned)irq6_asm, 0x08, 0x8E);
    idt_set_gate(39, (unsigned)irq7_asm, 0x08, 0x8E);
    idt_set_gate(40, (unsigned)irq8_asm, 0x08, 0x8E);
    idt_set_gate(41, (unsigned)irq9_asm, 0x08, 0x8E);
    idt_set_gate(42, (unsigned)irq10_asm, 0x08, 0x8E);
    idt_set_gate(43, (unsigned)irq11_asm, 0x08, 0x8E);
    idt_set_gate(44, (unsigned)irq12_asm, 0x08, 0x8E);
    idt_set_gate(45, (unsigned)irq13_asm, 0x08, 0x8E);
    idt_set_gate(46, (unsigned)irq14_asm, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15_asm, 0x08, 0x8E);
    
    idt_load((unsigned int)&ip);
}

// Memory management functions
void mm_init(unsigned int mem_size) {
    memory_size = mem_size;
    for (int i = 0; i < MAX_PAGES / 32; i++) {
        page_bitmap[i] = 0;
    }
}

void* alloc_page() {
    for (int i = 0; i < MAX_PAGES / 32; i++) {
        if (page_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                if (!(page_bitmap[i] & (1 << j))) {
                    page_bitmap[i] |= (1 << j);
                    return (void*)((i * 32 + j) * PAGE_SIZE);
                }
            }
        }
    }
    return 0;
}

void free_page(void* addr) {
    unsigned int page = (unsigned int)addr / PAGE_SIZE;
    unsigned int index = page / 32;
    unsigned int offset = page % 32;
    page_bitmap[index] &= ~(1 << offset);
}

// System call handler
void syscall_handler() {
    // Distro builders will implement their own syscalls
    terminal_writestring("System call called\n");
}

// Kernel main
void kernel_main() {
    // Initialize terminal
    terminal_initialize();
    terminal_writestring("PawNix kernel v0.1\n");
    terminal_writestring("Booting...\n");
    
    // Set up GDT
    terminal_writestring("Installing GDT... ");
    gdt_install();
    terminal_writestring("OK\n");
    
    // Set up IDT
    terminal_writestring("Installing IDT... ");
    idt_install();
    terminal_writestring("OK\n");
    
    // Initialize memory management
    terminal_writestring("Initializing memory manager... ");
    mm_init(128 * 1024 * 1024); // Assume 128MB for now
    terminal_writestring("OK\n");
    
    // Enable interrupts
    __asm__ volatile("sti");
    
    terminal_writestring("\nPawNix kernel ready.\n");
    terminal_writestring("PawNix v0.1.\n\n");
    
    // Main loop
    while(1) {
        __asm__ volatile("hlt");
    }
}