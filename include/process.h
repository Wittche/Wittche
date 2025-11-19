// Process Management header
#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

// Process states
#define PROCESS_STATE_READY     0
#define PROCESS_STATE_RUNNING   1
#define PROCESS_STATE_BLOCKED   2
#define PROCESS_STATE_TERMINATED 3

// Maximum number of processes
#define MAX_PROCESSES 16

// Process ID type
typedef uint32_t pid_t;

// CPU registers state for context switching
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, esp, ebp;
    uint32_t eip, eflags;
    uint32_t cr3;  // Page directory (for future use)
    uint16_t cs, ds, es, fs, gs, ss;
} __attribute__((packed)) cpu_state_t;

// Process Control Block (PCB)
typedef struct process {
    pid_t pid;                  // Process ID
    char name[32];              // Process name
    uint32_t state;             // Process state
    cpu_state_t cpu_state;      // Saved CPU state
    uint32_t stack_base;        // Stack base address
    uint32_t stack_size;        // Stack size
    uint32_t priority;          // Process priority (0-10, higher = more priority)
    uint32_t time_slice;        // CPU time slice
    uint32_t total_time;        // Total CPU time used
    struct process *next;       // Next process in list
} process_t;

/**
 * Process Management Functions
 */

// Initialize process management
void process_init(void);

// Create a new process with default priority
pid_t process_create(const char *name, void (*entry_point)(void), uint32_t stack_size);

// Create a new process with specified priority
pid_t process_create_with_priority(const char *name, void (*entry_point)(void),
                                    uint32_t stack_size, uint32_t priority);

// Set process priority (0-255, higher = more priority)
int process_set_priority(pid_t pid, uint32_t priority);

// Get process priority
int process_get_priority(pid_t pid);

// Kill a process
void process_kill(pid_t pid);

// Get current running process
process_t *process_current(void);

// Get process by PID
process_t *process_get(pid_t pid);

// Schedule next process (called by timer interrupt)
void process_schedule(void);

// Perform context switch
void process_switch(process_t *old_process, process_t *new_process);

// Yield CPU to next process
void process_yield(void);

// Get process list (for ps command)
process_t *process_list(void);

// Get process count
int process_count(void);

// Get process table (for ps command)
process_t **process_get_table(void);

#endif // PROCESS_H
