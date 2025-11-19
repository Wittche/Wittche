// Timer (PIT) driver implementation
#include "../include/timer.h"
#include "../include/ports.h"
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/types.h"
#include "../include/process.h"

// PIT (Programmable Interval Timer) I/O ports
#define PIT_CHANNEL0 0x40   // Channel 0 data port (IRQ0)
#define PIT_CHANNEL1 0x41   // Channel 1 data port
#define PIT_CHANNEL2 0x42   // Channel 2 data port
#define PIT_COMMAND  0x43   // Mode/Command register

// PIT frequency (oscillator frequency in Hz)
#define PIT_BASE_FREQUENCY 1193182

// System uptime in ticks (milliseconds if TIMER_FREQUENCY = 1000)
static volatile uint32_t system_ticks = 0;

/**
 * Initialize the PIT
 */
void timer_init(void) {
    // Calculate divisor for desired frequency
    // Frequency = PIT_BASE_FREQUENCY / divisor
    uint32_t divisor = PIT_BASE_FREQUENCY / TIMER_FREQUENCY;

    // Command byte:
    // Bits 7-6: Select channel (00 = channel 0)
    // Bits 5-4: Access mode (11 = lobyte/hibyte)
    // Bits 3-1: Operating mode (011 = square wave generator)
    // Bit 0:    BCD/Binary mode (0 = 16-bit binary)
    // Result: 0x36 = 00110110b
    uint8_t command = 0x36;

    // Send command byte
    outb(PIT_COMMAND, command);

    // Send divisor (low byte, then high byte)
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

    // Reset tick counter
    system_ticks = 0;

    // Print initialization message
    screen_write_color("[TIMER] ", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write("PIT initialized at ");
    screen_write_dec(TIMER_FREQUENCY);
    screen_write(" Hz (");
    screen_write_dec(1000 / TIMER_FREQUENCY);
    screen_write("ms per tick)\n");
}

/**
 * Timer interrupt handler
 * Called by IRQ0 handler on every timer tick
 */
void timer_handler(void) {
    system_ticks++;

    // Call process scheduler every 10 ticks (10ms time slice)
    // This enables preemptive multitasking
    if (system_ticks % 10 == 0) {
        process_schedule();
    }
}

/**
 * Get system uptime in ticks
 */
uint32_t timer_get_ticks(void) {
    return system_ticks;
}

/**
 * Get system uptime in seconds
 */
uint32_t timer_get_seconds(void) {
    return system_ticks / TIMER_FREQUENCY;
}

/**
 * Busy-wait for specified number of ticks
 * WARNING: This blocks the CPU
 */
void timer_wait(uint32_t ticks) {
    uint32_t start = system_ticks;
    while (system_ticks < start + ticks) {
        __asm__ __volatile__("hlt");  // Wait for next interrupt
    }
}

/**
 * Format uptime as "HH:MM:SS"
 */
void timer_format_uptime(char *buffer) {
    uint32_t seconds = timer_get_seconds();

    uint32_t hours = seconds / 3600;
    uint32_t minutes = (seconds % 3600) / 60;
    uint32_t secs = seconds % 60;

    // Format: HH:MM:SS
    // Convert each component to string
    buffer[0] = '0' + (hours / 10) % 10;
    buffer[1] = '0' + (hours % 10);
    buffer[2] = ':';
    buffer[3] = '0' + (minutes / 10);
    buffer[4] = '0' + (minutes % 10);
    buffer[5] = ':';
    buffer[6] = '0' + (secs / 10);
    buffer[7] = '0' + (secs % 10);
    buffer[8] = '\0';
}
