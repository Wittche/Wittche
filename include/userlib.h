#ifndef USERLIB_H
#define USERLIB_H

#include "types.h"

// User-space library for making system calls
// These functions wrap INT 0x80 syscalls

// Exit process
void exit(int exit_code);

// Write to stdout
int write(int fd, const char *buf, uint32_t count);

// Read from stdin
int read(int fd, char *buf, uint32_t count);

// Get process ID
int getpid(void);

// Sleep for milliseconds
int sleep_ms(uint32_t milliseconds);

// Yield CPU
void yield(void);

// Send IPC message
int send_msg(int target_pid, const char *message, uint32_t length);

// Receive IPC message
int recv_msg(char *buffer, uint32_t max_length, int *sender);

#endif // USERLIB_H
