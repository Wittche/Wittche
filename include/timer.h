// Timer (PIT) driver header
#ifndef TIMER_H
#define TIMER_H

#include "types.h"

// Timer frequency in Hz
#define TIMER_FREQUENCY 1000  // 1000 Hz = 1ms per tick

/**
 * Initialize the Programmable Interval Timer (PIT)
 * Sets up IRQ0 to fire at TIMER_FREQUENCY Hz
 */
void timer_init(void);

/**
 * Timer interrupt handler
 * Called by IRQ0 handler
 */
void timer_handler(void);

/**
 * Get system uptime in ticks (milliseconds)
 */
uint32_t timer_get_ticks(void);

/**
 * Get system uptime in seconds
 */
uint32_t timer_get_seconds(void);

/**
 * Sleep for specified number of ticks (busy wait)
 * WARNING: This is a busy-wait, not a real sleep
 */
void timer_wait(uint32_t ticks);

/**
 * Format uptime as "HH:MM:SS"
 * buffer must be at least 9 bytes
 */
void timer_format_uptime(char *buffer);

#endif // TIMER_H
