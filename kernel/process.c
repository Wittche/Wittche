// Process Management implementation
#include "../include/process.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/heap.h"
#include "../include/string.h"

// Process table
static process_t *process_table[MAX_PROCESSES];
static int process_count_val = 0;
static pid_t next_pid = 1;

// Current running process
static process_t *current_process = NULL;

// Idle process (runs when no other process is ready)
static process_t *idle_process = NULL;

/**
 * Idle process function - runs when no other process is ready
 */
static void idle_task(void) {
    while (1) {
        __asm__ __volatile__("hlt");  // Halt until interrupt
    }
}

/**
 * Initialize process management system
 */
void process_init(void) {
    // Clear process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i] = NULL;
    }

    screen_write_color("[PROCESS] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("Process management initialized\n");

    // Create idle process (PID 0)
    idle_process = (process_t *)kmalloc(sizeof(process_t));
    if (!idle_process) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate idle process\n");
        return;
    }

    idle_process->pid = 0;
    strcpy(idle_process->name, "idle");
    idle_process->state = PROCESS_STATE_READY;
    idle_process->priority = 0;  // Lowest priority
    idle_process->time_slice = 10;
    idle_process->total_time = 0;
    idle_process->next = NULL;

    // Allocate stack for idle process
    idle_process->stack_size = 4096;  // 4KB stack
    idle_process->stack_base = (uint32_t)kmalloc(idle_process->stack_size);

    // Setup idle process stack and entry point
    uint32_t *stack = (uint32_t *)(idle_process->stack_base + idle_process->stack_size);
    stack--;  // Move to top of stack
    *stack = (uint32_t)idle_task;  // Entry point

    // Initialize CPU state
    idle_process->cpu_state.eip = (uint32_t)idle_task;
    idle_process->cpu_state.esp = (uint32_t)stack;
    idle_process->cpu_state.ebp = idle_process->cpu_state.esp;
    idle_process->cpu_state.eflags = 0x202;  // IF (interrupts enabled)
    idle_process->cpu_state.cs = 0x08;  // Kernel code segment
    idle_process->cpu_state.ds = 0x10;  // Kernel data segment
    idle_process->cpu_state.es = 0x10;
    idle_process->cpu_state.fs = 0x10;
    idle_process->cpu_state.gs = 0x10;
    idle_process->cpu_state.ss = 0x10;

    process_table[0] = idle_process;
    current_process = idle_process;
    process_count_val = 1;

    kprintf("          Idle process created (PID 0)\n");
}

/**
 * Create a new process
 */
pid_t process_create(const char *name, void (*entry_point)(void), uint32_t stack_size) {
    if (process_count_val >= MAX_PROCESSES) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Process table full\n");
        return 0;
    }

    // Allocate PCB
    process_t *process = (process_t *)kmalloc(sizeof(process_t));
    if (!process) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate PCB\n");
        return 0;
    }

    // Allocate stack
    process->stack_base = (uint32_t)kmalloc(stack_size);
    if (!process->stack_base) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate stack\n");
        kfree(process);
        return 0;
    }

    // Initialize PCB
    process->pid = next_pid++;
    strncpy(process->name, name, 31);
    process->name[31] = '\0';
    process->state = PROCESS_STATE_READY;
    process->stack_size = stack_size;
    process->priority = 5;  // Default priority
    process->time_slice = 10;  // Default time slice
    process->total_time = 0;
    process->next = NULL;

    // Setup stack and entry point
    uint32_t *stack = (uint32_t *)(process->stack_base + stack_size);
    stack--;  // Move to top of stack
    *stack = (uint32_t)entry_point;  // Entry point

    // Initialize CPU state
    process->cpu_state.eip = (uint32_t)entry_point;
    process->cpu_state.esp = (uint32_t)stack;
    process->cpu_state.ebp = process->cpu_state.esp;
    process->cpu_state.eflags = 0x202;  // IF (interrupts enabled)
    process->cpu_state.cs = 0x08;  // Kernel code segment
    process->cpu_state.ds = 0x10;  // Kernel data segment
    process->cpu_state.es = 0x10;
    process->cpu_state.fs = 0x10;
    process->cpu_state.gs = 0x10;
    process->cpu_state.ss = 0x10;

    // Add to process table
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] == NULL) {
            process_table[i] = process;
            process_count_val++;
            kprintf("[PROCESS] Created process '%s' (PID %d)\n", name, process->pid);
            return process->pid;
        }
    }

    // Should not reach here
    kfree((void *)process->stack_base);
    kfree(process);
    return 0;
}

/**
 * Kill a process
 */
void process_kill(pid_t pid) {
    if (pid == 0) {
        // Cannot kill idle process
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Cannot kill idle process\n");
        return;
    }

    // Find process
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] && process_table[i]->pid == pid) {
            process_t *process = process_table[i];

            // If killing current process, schedule next
            if (process == current_process) {
                process->state = PROCESS_STATE_TERMINATED;
                process_schedule();  // Will switch to another process
            }

            // Free resources
            kfree((void *)process->stack_base);
            kfree(process);
            process_table[i] = NULL;
            process_count_val--;

            kprintf("[PROCESS] Killed process PID %d\n", pid);
            return;
        }
    }

    kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                 "[PROCESS ERROR] Process PID %d not found\n", pid);
}

/**
 * Get current running process
 */
process_t *process_current(void) {
    return current_process;
}

/**
 * Get process by PID
 */
process_t *process_get(pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] && process_table[i]->pid == pid) {
            return process_table[i];
        }
    }
    return NULL;
}

/**
 * Round-robin scheduler - selects next process to run
 */
void process_schedule(void) {
    if (process_count_val == 0) return;

    // Save current process state if it's still running
    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
    }

    // Find next ready process (round-robin)
    process_t *next_process = NULL;
    int start_index = -1;

    // Find current process index
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] == current_process) {
            start_index = i;
            break;
        }
    }

    // Search for next ready process
    for (int offset = 1; offset <= MAX_PROCESSES; offset++) {
        int i = (start_index + offset) % MAX_PROCESSES;
        if (process_table[i] &&
            process_table[i]->state == PROCESS_STATE_READY) {
            next_process = process_table[i];
            break;
        }
    }

    // If no ready process, use idle
    if (!next_process) {
        next_process = idle_process;
    }

    // If switching to different process
    if (next_process != current_process) {
        process_t *old_process = current_process;
        current_process = next_process;
        current_process->state = PROCESS_STATE_RUNNING;

        // Perform context switch
        process_switch(old_process, current_process);
    } else {
        current_process->state = PROCESS_STATE_RUNNING;
    }
}

/**
 * Get process list (for ps command)
 */
process_t *process_list(void) {
    // Return first process (caller will iterate through ->next)
    // For now, we'll just return the table (simplified)
    return NULL;  // We'll access process_table directly in ps command
}

/**
 * Get process count
 */
int process_count(void) {
    return process_count_val;
}

/**
 * Get process table (for ps command)
 */
process_t **process_get_table(void) {
    return process_table;
}
