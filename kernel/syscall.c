// System call implementation
// Provides interface between user mode and kernel mode

#include "../include/syscall.h"
#include "../include/process.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/idt.h"

// System call table
static syscall_handler_t syscall_table[SYSCALL_MAX] = {
    sys_exit,    // 0
    sys_write,   // 1
    sys_read,    // 2
    sys_getpid,  // 3
    sys_sleep,   // 4
    sys_yield,   // 5
    sys_send,    // 6
    sys_recv     // 7
};

// External interrupt handler from syscall.asm
extern void syscall_interrupt_handler(void);

/**
 * Initialize system call interface
 */
void syscall_init(void) {
    // Register INT 0x80 for system calls
    // DPL=3 means usermode can call this interrupt
    idt_set_gate(0x80, (uint32_t)syscall_interrupt_handler, 0x08, 0xEE);

    kprintf("[SYSCALL] System call interface initialized (INT 0x80)\n");
    kprintf("[SYSCALL] Available syscalls: %d\n", SYSCALL_MAX);
}

/**
 * System call dispatcher
 * Called from assembly interrupt handler with syscall number and arguments
 */
int syscall_dispatch(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    // Validate syscall number
    if (syscall_num >= SYSCALL_MAX) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[SYSCALL ERROR] Invalid syscall number: %d\n", syscall_num);
        return -1;
    }

    // Call the appropriate system call handler
    syscall_handler_t handler = syscall_table[syscall_num];
    if (handler) {
        return handler(arg1, arg2, arg3, arg4);
    }

    return -1;
}

/**
 * SYS_EXIT - Exit current process
 * arg1 = exit_code
 */
int sys_exit(uint32_t exit_code, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg2;
    (void)arg3;
    (void)arg4;

    process_t *current = process_current();
    if (current) {
        kprintf("[SYSCALL] Process %d exiting with code %d\n",
                current->pid, exit_code);
    }

    process_exit();

    // Should never return
    return 0;
}

/**
 * SYS_WRITE - Write to screen (stdout)
 * arg1 = fd (file descriptor, ignored for now)
 * arg2 = buffer (pointer to string)
 * arg3 = count (number of bytes)
 */
int sys_write(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t arg4) {
    (void)fd;    // Ignore fd for now, always write to screen
    (void)arg4;

    if (!buffer || count == 0) {
        return -1;
    }

    char *str = (char *)buffer;

    // Write each character to screen
    for (uint32_t i = 0; i < count; i++) {
        if (str[i] == '\0') break;

        // Write single character as string
        char temp[2] = {str[i], '\0'};
        screen_write(temp);
    }

    return (int)count;
}

/**
 * SYS_READ - Read from keyboard (stdin)
 * arg1 = fd (file descriptor, ignored for now)
 * arg2 = buffer (pointer to buffer)
 * arg3 = count (max bytes to read)
 * Returns: number of bytes read
 */
int sys_read(uint32_t fd, uint32_t buffer, uint32_t count, uint32_t arg4) {
    (void)fd;
    (void)buffer;
    (void)count;
    (void)arg4;

    // TODO: Implement keyboard read
    // For now, just return 0 (no data)
    return 0;
}

/**
 * SYS_GETPID - Get current process ID
 * Returns: current PID
 */
int sys_getpid(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg1;
    (void)arg2;
    (void)arg3;
    (void)arg4;

    process_t *current = process_current();
    if (current) {
        return current->pid;
    }

    return -1;
}

/**
 * SYS_SLEEP - Sleep for specified milliseconds
 * arg1 = milliseconds
 */
int sys_sleep(uint32_t milliseconds, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg2;
    (void)arg3;
    (void)arg4;

    process_sleep(milliseconds);
    return 0;
}

/**
 * SYS_YIELD - Yield CPU to next process
 */
int sys_yield(uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4) {
    (void)arg1;
    (void)arg2;
    (void)arg3;
    (void)arg4;

    process_yield();
    return 0;
}

/**
 * SYS_SEND - Send IPC message
 * arg1 = target_pid
 * arg2 = message buffer
 * arg3 = message length
 */
int sys_send(uint32_t target_pid, uint32_t message, uint32_t length, uint32_t arg4) {
    (void)arg4;

    if (!message || length == 0) {
        return -1;
    }

    return process_send_message((pid_t)target_pid, (const char *)message, length);
}

/**
 * SYS_RECV - Receive IPC message
 * arg1 = buffer
 * arg2 = max_length
 * arg3 = sender_ptr (optional, can be NULL)
 */
int sys_recv(uint32_t buffer, uint32_t max_length, uint32_t sender_ptr, uint32_t arg4) {
    (void)arg4;

    if (!buffer || max_length == 0) {
        return -1;
    }

    pid_t sender;
    int result = process_receive_message((char *)buffer, max_length, &sender);

    // If caller wants sender PID, write it
    if (sender_ptr && result > 0) {
        *((pid_t *)sender_ptr) = sender;
    }

    return result;
}
