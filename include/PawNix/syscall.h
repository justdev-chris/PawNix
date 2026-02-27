#ifndef PAWNIX_SYSCALL_H
#define PAWNIX_SYSCALL_H

// System call numbers
#define SYS_EXIT    0
#define SYS_WRITE   1
#define SYS_READ    2
#define SYS_OPEN    3
#define SYS_CLOSE   4
#define SYS_MMAP    5
#define SYS_SLEEP   6
#define SYS_GETPID  7

// Calling convention:
// eax = syscall number
// ebx, ecx, edx, esi, edi = args (up to 5)
// int 0x80 triggers syscall
// Return value in eax

static inline int syscall(int num, int arg1, int arg2, int arg3) {
    int ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
        : "memory"
    );
    return ret;
}

// Wrappers for common syscalls
static inline void exit(int status) {
    syscall(SYS_EXIT, status, 0, 0);
}

static inline int write(int fd, const void* buf, int count) {
    return syscall(SYS_WRITE, fd, (int)buf, count);
}

static inline int read(int fd, void* buf, int count) {
    return syscall(SYS_READ, fd, (int)buf, count);
}

static inline int open(const char* path, int flags) {
    return syscall(SYS_OPEN, (int)path, flags, 0);
}

static inline int close(int fd) {
    return syscall(SYS_CLOSE, fd, 0, 0);
}

static inline int sleep(int ms) {
    return syscall(SYS_SLEEP, ms, 0, 0);
}

static inline int getpid(void) {
    return syscall(SYS_GETPID, 0, 0, 0);
}

#endif