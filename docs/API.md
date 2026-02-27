# PawNix Kernel API

## System Calls

PawNix uses `int 0x80` for system calls.

**Calling Convention:**
- `eax` = syscall number
- `ebx` = arg1
- `ecx` = arg2
- `edx` = arg3
- `esi` = arg4 (if needed)
- `edi` = arg5 (if needed)
- Return value in `eax`

**Available Syscalls:**

| Num | Name    | Args                    | Description          |
|-----|---------|-------------------------|----------------------|
| 0   | exit    | int status              | Exit process         |
| 1   | write   | int fd, void* buf, int count | Write to fd    |
| 2   | read    | int fd, void* buf, int count | Read from fd     |
| 3   | open    | char* path, int flags   | Open file            |
| 4   | close   | int fd                  | Close file           |
| 5   | mmap    | void* addr, int len     | Map memory           |
| 6   | sleep   | int ms                  | Sleep for ms         |
| 7   | getpid  | -                       | Get process ID       |

## Memory Layout

- Kernel loaded at: `0x100000`
- User space starts: `0x40000000`
- Stack: grows down from `0x80000000`

## Interrupts

- IRQ0 (timer): every 10ms
- IRQ1 (keyboard): on key press
- IRQ14/15: ATA disk

## Building Userspace

1. Use `i686-elf-gcc` (or any cross-compiler)
2. Compile with `-ffreestanding -nostdlib`
3. Link with PawNix syscall header
4. Resulting binary must be in flat format

Example minimal program:
```
#include <PawNix/syscall.h>

void _start() {
    write(1, "Hello from PawNix\n", 18);
    exit(0);
}
```

Hardware Support

· x86 (32-bit) only for now
· Requires: 32MB RAM minimum
· VGA text mode (80x25)
· PS/2 keyboard
· ATA hard disks (PIO mode)
· Serial port (COM1) for debug

Boot Protocol

PawNix expects to be loaded at 0x100000 in 32-bit protected mode.
Can be booted by:

· GRUB (multiboot compliant)
· Custom bootloader
· Direct BIOS load (first 512 bytes are boot sector)