# Building a distro with PawNix

## What you get
- 32-bit x86 kernel
- Basic syscalls (exit, write, read, open, close, mmap, sleep, getpid)
- Timer, keyboard, ATA disk support
- VGA text mode

## What you need to add
- Bootloader (GRUB recommended)
- C library (musl is easy to port)
- Init system
- Userland apps (shell, etc)

## Building PawNix
```

make

```
Output: `PawNix.bin`

## Making your distro
1. Copy PawNix.bin to your sysroot/boot/
2. Copy include/PawNix/ to your cross-compiler's sysroot/usr/include/
3. Build your userspace against PawNix syscalls
4. Create initrd with your programs
5. Build ISO with GRUB

## Syscall quick ref
See API.md for full docs.