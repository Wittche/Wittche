#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"

// System call numbers
#define SYS_EXIT    0
#define SYS_WRITE   1
#define SYS_READ    2
#define SYS_GETPID  3
#define SYS_SLEEP   4
#define SYS_YIELD   5
#define SYS_SEND    6
#define SYS_RECV    7

// Maximum number of system calls
#define SYSCALL_MAX 8

// System call handler type
typedef int (*syscall_handler_t)(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// Initialize system call interface
void syscall_init(void);

// System call dispatcher (called from interrupt handler)
int syscall_dispatch(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);

// Actual system call implementations
int sys_exit(uint32_t exit_code, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int sys_write(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t arg4);
int sys_read(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t arg4);
int sys_getpid(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int sys_sleep(uint32_t milliseconds, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int sys_yield(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4);
int sys_send(uint32_t target_pid, uint32_t message, uint32_t length, uint32_t arg4);
int sys_recv(uint32_t buffer, uint32_t max_length, uint32_t sender_ptr, uint32_t arg4);

#endif // SYSCALL_H
