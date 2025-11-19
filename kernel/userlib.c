// User library - System call wrappers
// These functions provide a C interface to kernel syscalls

#include "../include/userlib.h"
#include "../include/syscall.h"

// Helper macro for making syscalls
// Syscall convention: EAX=syscall_num, EBX=arg1, ECX=arg2, EDX=arg3, ESI=arg4
#define SYSCALL0(num) ({                    \
    int ret;                                 \
    __asm__ volatile (                       \
        "int $0x80"                          \
        : "=a" (ret)                         \
        : "0" (num)                          \
        : "memory"                           \
    );                                       \
    ret;                                     \
})

#define SYSCALL1(num, arg1) ({               \
    int ret;                                 \
    __asm__ volatile (                       \
        "int $0x80"                          \
        : "=a" (ret)                         \
        : "0" (num), "b" ((uint32_t)(arg1))  \
        : "memory"                           \
    );                                       \
    ret;                                     \
})

#define SYSCALL2(num, arg1, arg2) ({         \
    int ret;                                 \
    __asm__ volatile (                       \
        "int $0x80"                          \
        : "=a" (ret)                         \
        : "0" (num), "b" ((uint32_t)(arg1)), "c" ((uint32_t)(arg2)) \
        : "memory"                           \
    );                                       \
    ret;                                     \
})

#define SYSCALL3(num, arg1, arg2, arg3) ({   \
    int ret;                                 \
    __asm__ volatile (                       \
        "int $0x80"                          \
        : "=a" (ret)                         \
        : "0" (num), "b" ((uint32_t)(arg1)), "c" ((uint32_t)(arg2)), \
          "d" ((uint32_t)(arg3))             \
        : "memory"                           \
    );                                       \
    ret;                                     \
})

#define SYSCALL4(num, arg1, arg2, arg3, arg4) ({ \
    int ret;                                 \
    __asm__ volatile (                       \
        "int $0x80"                          \
        : "=a" (ret)                         \
        : "0" (num), "b" ((uint32_t)(arg1)), "c" ((uint32_t)(arg2)), \
          "d" ((uint32_t)(arg3)), "S" ((uint32_t)(arg4)) \
        : "memory"                           \
    );                                       \
    ret;                                     \
})

/**
 * Exit process
 */
void exit(int exit_code) {
    SYSCALL1(SYS_EXIT, exit_code);
}

/**
 * Write to file descriptor (stdout = 1)
 */
int write(int fd, const char *buf, uint32_t count) {
    return SYSCALL3(SYS_WRITE, fd, buf, count);
}

/**
 * Read from file descriptor (stdin = 0)
 */
int read(int fd, char *buf, uint32_t count) {
    return SYSCALL3(SYS_READ, fd, buf, count);
}

/**
 * Get process ID
 */
int getpid(void) {
    return SYSCALL0(SYS_GETPID);
}

/**
 * Sleep for milliseconds
 */
int sleep_ms(uint32_t milliseconds) {
    return SYSCALL1(SYS_SLEEP, milliseconds);
}

/**
 * Yield CPU
 */
void yield(void) {
    SYSCALL0(SYS_YIELD);
}

/**
 * Send IPC message
 */
int send_msg(int target_pid, const char *message, uint32_t length) {
    return SYSCALL3(SYS_SEND, target_pid, message, length);
}

/**
 * Receive IPC message
 */
int recv_msg(char *buffer, uint32_t max_length, int *sender) {
    return SYSCALL3(SYS_RECV, buffer, max_length, sender);
}
