// Process Management implementation
#include "../include/process.h"
#include "../include/screen.h"
#include "../include/kprintf.h"
#include "../include/heap.h"
#include "../include/string.h"
#include "../include/timer.h"
#include "../include/gdt.h"
#include "../include/paging.h"
#include "../include/pmm.h"

// External assembly function to enter user mode
extern void enter_usermode(void (*entry_point)(void), uint32_t user_stack);

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
    idle_process->page_directory = paging_get_directory();  // Use kernel page directory

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
    process->user_stack = 0;  // No user stack (kernel mode)
    process->kernel_stack = process->stack_base + stack_size;  // Kernel stack top
    process->is_user_mode = 0;  // Kernel mode by default
    process->priority = 5;  // Default priority
    process->time_slice = 10;  // Default time slice
    process->total_time = 0;
    process->wake_time = 0;  // Not sleeping
    process->next = NULL;

    // Initialize IPC message queue
    process->msg_queue = (message_queue_t *)kmalloc(sizeof(message_queue_t));
    if (!process->msg_queue) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate message queue\n");
        kfree((void *)process->stack_base);
        kfree(process);
        return 0;
    }
    process->msg_queue->head = 0;
    process->msg_queue->tail = 0;
    process->msg_queue->count = 0;

    // Use kernel page directory for kernel mode processes
    process->page_directory = paging_get_directory();

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
 * Create a new process with specified priority
 */
pid_t process_create_with_priority(const char *name, void (*entry_point)(void),
                                    uint32_t stack_size, uint32_t priority) {
    pid_t pid = process_create(name, entry_point, stack_size);
    if (pid > 0) {
        process_t *proc = process_get(pid);
        if (proc) {
            proc->priority = priority > 255 ? 255 : priority;  // Clamp to 0-255
        }
    }
    return pid;
}

/**
 * User mode entry wrapper
 * This wrapper calls enter_usermode to switch to Ring 3
 */
static void user_mode_entry_wrapper(void) {
    // Get current process
    process_t *proc = current_process;
    if (!proc || !proc->is_user_mode) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Invalid user mode process\n");
        process_exit();
        return;
    }

    // Get entry point from CPU state
    void (*entry_point)(void) = (void (*)(void))proc->cpu_state.eip;
    uint32_t user_stack = proc->user_stack;

    kprintf("[USERMODE] Switching to Ring 3: entry=0x%x, stack=0x%x\n",
            (uint32_t)entry_point, user_stack);

    // Jump to Ring 3
    enter_usermode(entry_point, user_stack);

    // Should never reach here
    kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                 "[USERMODE ERROR] Returned from enter_usermode!\n");
    process_exit();
}

/**
 * Create a new user mode process (Ring 3)
 */
pid_t process_create_user_mode(const char *name, void (*entry_point)(void),
                               uint32_t stack_size, uint32_t priority) {
    // Create process normally
    pid_t pid = process_create(name, user_mode_entry_wrapper, stack_size);
    if (pid == 0) {
        return 0;  // Failed to create
    }

    // Get the created process
    process_t *proc = process_get(pid);
    if (!proc) {
        return 0;
    }

    // Mark as user mode process EARLY to prevent scheduler issues
    proc->is_user_mode = 1;

    // Allocate physical memory for user stack
    uint32_t stack_physical = pmm_alloc_page();  // Allocate at least one page
    if (!stack_physical) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate user stack page\n");
        process_kill(pid);
        return 0;
    }

    // Create separate page directory for user process
    proc->page_directory = paging_create_user_directory();
    if (!proc->page_directory) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to create page directory\n");
        process_kill(pid);
        return 0;
    }

    // Allocate physical memory for user code (4KB = 1 page)
    uint32_t code_physical = pmm_alloc_page();
    if (!code_physical) {
        kprintf_color(MAKE_COLOR(COLOR_RED, COLOR_BLACK),
                     "[PROCESS ERROR] Failed to allocate user code page\n");
        process_kill(pid);
        return 0;
    }

    // Copy entry point code to allocated page
    // We'll copy 4KB (assuming code is smaller than 1 page)
    memcpy((void *)code_physical, (void *)entry_point, PAGE_SIZE);

    // Map user code to virtual address (USER_CODE_BASE = 0x40000000)
    paging_map_user_code(proc->page_directory, USER_CODE_BASE, code_physical, PAGE_SIZE);

    // Map user stack to virtual address (USER_STACK_BASE = 0x80000000)
    // Stack grows down, so we map at the base
    paging_map_user_code(proc->page_directory, USER_STACK_BASE - PAGE_SIZE, stack_physical, PAGE_SIZE);

    // Set user stack top (stack grows down from USER_STACK_BASE)
    proc->user_stack = USER_STACK_BASE;

    kprintf("[USERMODE] Mapped user code: virt=0x%x, phys=0x%x\n", USER_CODE_BASE, code_physical);
    kprintf("[USERMODE] Mapped user stack: virt=0x%x, phys=0x%x\n", USER_STACK_BASE - PAGE_SIZE, stack_physical);

    // Set priority
    proc->priority = priority > 255 ? 255 : priority;

    // Set entry point to user virtual address
    proc->cpu_state.eip = USER_CODE_BASE;

    // Update segments for user mode
    proc->cpu_state.cs = 0x1B;  // User code segment (0x18 | 0x03)
    proc->cpu_state.ds = 0x23;  // User data segment (0x20 | 0x03)
    proc->cpu_state.es = 0x23;
    proc->cpu_state.fs = 0x23;
    proc->cpu_state.gs = 0x23;
    proc->cpu_state.ss = 0x23;

    kprintf("[PROCESS] Created user mode process '%s' (PID %d, Ring 3)\n", name, pid);
    return pid;
}

/**
 * Set process priority
 */
int process_set_priority(pid_t pid, uint32_t priority) {
    process_t *proc = process_get(pid);
    if (!proc) {
        return -1;  // Process not found
    }

    proc->priority = priority > 255 ? 255 : priority;  // Clamp to 0-255
    return 0;
}

/**
 * Get process priority
 */
int process_get_priority(pid_t pid) {
    process_t *proc = process_get(pid);
    if (!proc) {
        return -1;  // Process not found
    }
    return proc->priority;
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
            if (process->msg_queue) {
                kfree(process->msg_queue);
            }

            // Free page directory if it's a user mode process
            if (process->is_user_mode && process->page_directory) {
                paging_free_directory(process->page_directory);
            }

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
 * Exit current process (terminate self)
 */
void process_exit(void) {
    if (!current_process) {
        return;  // No process running
    }

    if (current_process->pid == 0) {
        // Idle process cannot exit
        return;
    }

    pid_t pid = current_process->pid;

    // Kill current process
    process_kill(pid);

    // This point should never be reached as process_kill
    // will schedule another process, but just in case:
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

/**
 * Put current process to sleep for specified milliseconds
 */
void process_sleep(uint32_t ms) {
    if (!current_process) {
        return;  // No process running
    }

    if (current_process->pid == 0) {
        // Idle process cannot sleep
        return;
    }

    // Calculate wake up time in ticks (1000 Hz timer = 1 tick per ms)
    uint32_t sleep_ticks = ms;
    current_process->wake_time = timer_get_ticks() + sleep_ticks;
    current_process->state = PROCESS_STATE_BLOCKED;

    // Immediately schedule another process
    process_schedule();
}

/**
 * Wake up a sleeping process
 */
int process_wake(pid_t pid) {
    process_t *proc = process_get(pid);
    if (!proc) {
        return -1;  // Process not found
    }

    if (proc->state == PROCESS_STATE_BLOCKED) {
        proc->state = PROCESS_STATE_READY;
        proc->wake_time = 0;
        return 0;  // Success
    }

    return -2;  // Process not sleeping
}

/**
 * Check and wake up sleeping processes (called by timer every tick)
 */
void process_check_sleeping(void) {
    uint32_t current_ticks = timer_get_ticks();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] &&
            process_table[i]->state == PROCESS_STATE_BLOCKED &&
            process_table[i]->wake_time > 0) {

            // Check if it's time to wake up
            if (current_ticks >= process_table[i]->wake_time) {
                process_table[i]->state = PROCESS_STATE_READY;
                process_table[i]->wake_time = 0;
            }
        }
    }
}

/**
 * Send message to another process (IPC)
 */
int process_send_message(pid_t target_pid, const char *message, uint32_t length) {
    if (!message || length == 0 || length > MAX_MESSAGE_SIZE) {
        return -1;  // Invalid parameters
    }

    // Find target process
    process_t *target = process_get(target_pid);
    if (!target || !target->msg_queue) {
        return -2;  // Process not found or no message queue
    }

    // Check if queue is full
    if (target->msg_queue->count >= MESSAGE_QUEUE_SIZE) {
        return -3;  // Queue full
    }

    // Get tail position for new message
    uint32_t tail = target->msg_queue->tail;
    ipc_message_t *msg = &target->msg_queue->messages[tail];

    // Copy message data
    msg->sender = current_process ? current_process->pid : 0;
    msg->length = length > MAX_MESSAGE_SIZE ? MAX_MESSAGE_SIZE : length;
    memcpy(msg->data, message, msg->length);

    // Update queue pointers
    target->msg_queue->tail = (tail + 1) % MESSAGE_QUEUE_SIZE;
    target->msg_queue->count++;

    // If target process is blocked waiting for messages, wake it up
    if (target->state == PROCESS_STATE_BLOCKED && target->wake_time == 0) {
        target->state = PROCESS_STATE_READY;
    }

    return 0;  // Success
}

/**
 * Receive message from current process queue (IPC)
 */
int process_receive_message(char *buffer, uint32_t max_length, pid_t *sender) {
    if (!current_process || !current_process->msg_queue) {
        return -1;  // No current process or message queue
    }

    if (!buffer || max_length == 0) {
        return -2;  // Invalid buffer
    }

    message_queue_t *queue = current_process->msg_queue;

    // Check if queue is empty
    if (queue->count == 0) {
        return -3;  // No messages available
    }

    // Get head message
    uint32_t head = queue->head;
    ipc_message_t *msg = &queue->messages[head];

    // Copy message data to buffer
    uint32_t copy_length = msg->length < max_length ? msg->length : max_length;
    memcpy(buffer, msg->data, copy_length);

    // Set sender if requested
    if (sender) {
        *sender = msg->sender;
    }

    // Update queue pointers
    queue->head = (head + 1) % MESSAGE_QUEUE_SIZE;
    queue->count--;

    return (int)copy_length;  // Return number of bytes copied
}

/**
 * Check if current process has messages waiting (IPC)
 */
int process_has_messages(void) {
    if (!current_process || !current_process->msg_queue) {
        return 0;  // No messages
    }

    return current_process->msg_queue->count;
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
 * Priority-based scheduler - selects highest priority ready process
 * Priority range: 0-255 (higher value = higher priority)
 */
void process_schedule(void) {
    if (process_count_val == 0) return;

    // Save current process state if it's still running
    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
    }

    // Find highest priority ready process
    process_t *next_process = NULL;
    uint32_t highest_priority = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i] &&
            process_table[i]->state == PROCESS_STATE_READY) {

            // Select process with highest priority
            if (next_process == NULL || process_table[i]->priority > highest_priority) {
                next_process = process_table[i];
                highest_priority = process_table[i]->priority;
            }
            // If same priority, use round-robin (favor process with lower index)
            else if (process_table[i]->priority == highest_priority) {
                // Check if this is the "next" process after current (round-robin tie-breaker)
                if (current_process) {
                    for (int j = 0; j < MAX_PROCESSES; j++) {
                        if (process_table[j] == current_process) {
                            // Prefer process that comes after current in table
                            if (i > j && process_table[i]->priority == highest_priority) {
                                next_process = process_table[i];
                            }
                            break;
                        }
                    }
                }
            }
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

        // Set TSS kernel stack for privilege level switching
        // This is used when user mode process makes syscall
        tss_set_kernel_stack(current_process->kernel_stack);

        // Switch page directory if process has its own
        if (current_process->page_directory) {
            paging_switch_directory(current_process->page_directory);
        }

        // Perform context switch
        process_switch(old_process, current_process);
    } else {
        current_process->state = PROCESS_STATE_RUNNING;
    }
}

/**
 * Yield CPU to next process
 * Voluntarily gives up the CPU and triggers scheduling
 */
void process_yield(void) {
    // Simply trigger the scheduler
    // Current process will be marked READY and scheduler will pick next process
    process_schedule();
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
