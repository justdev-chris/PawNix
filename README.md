# PawNix - A simple kernel

PawNix is a 32-bit x86 kernel. Nothing more. Build your distro on top of it.

## What you get
- x86 kernel with basic syscalls
- VGA text output
- Keyboard input
- ATA disk support
- Timer (10ms ticks)
- Memory management

## Build
```

make

```
Output: `PawNix.bin`

## System calls
| Num | Name    | Args                    |
|-----|---------|-------------------------|
| 0   | exit    | int status              |
| 1   | write   | int fd, void* buf, int count |
| 2   | read    | int fd, void* buf, int count |
| 3   | open    | char* path, int flags   |
| 4   | close   | int fd                  |
| 5   | mmap    | void* addr, int len     |
| 6   | sleep   | int ms                  |
| 7   | getpid  | -                       |

Call with `int 0x80`, eax=syscall number, ebx/ecx/edx=args.

## Making a distro
1. Copy `PawNix.bin` to your boot directory
2. Copy `include/PawNix/` to your toolchain sysroot
3. Build userspace against PawNix syscalls
4. Create initrd with your programs
5. Boot with GRUB (multiboot compatible)

## Minimal userspace program
```c
#include <PawNix/syscall.h>

void _start() {
    write(1, "Hello\n", 6);
    exit(0);
}
```

Compile: i686-elf-gcc -ffreestanding -nostdlib -Iinclude prog.c -o prog

Requirements

· 32MB RAM minimum
· x86 compatible
· VGA text mode
· PS/2 keyboard (optional)
· ATA disk (optional)

License

MIT License

Author

justdev-chris
