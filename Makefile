# PawNix Makefile
ASM = nasm
CC = gcc
LD = ld

ASMFLAGS = -f elf32
CFLAGS = -ffreestanding -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -m32 -Iinclude
LDFLAGS = -T linker.ld -m elf_i386

OBJS = src/boot/boot.o src/boot/interrupts.o src/kernel.o

all: PawNix.bin

src/boot/boot.o: src/boot/boot.asm
	$(ASM) $(ASMFLAGS) src/boot/boot.asm -o src/boot/boot.o

src/boot/interrupts.o: src/boot/interrupts.asm
	$(ASM) $(ASMFLAGS) src/boot/interrupts.asm -o src/boot/interrupts.o

src/kernel.o: src/kernel.c src/kernel.h
	$(CC) $(CFLAGS) -c src/kernel.c -o src/kernel.o

PawNix.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) PawNix.bin

run: PawNix.bin
	qemu-system-i386 -kernel PawNix.bin

.PHONY: clean run