// Port I/O functions
#ifndef PORTS_H
#define PORTS_H

#include "types.h"

// Read byte from port
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ __volatile__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write byte to port
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Read word from port
static inline uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ __volatile__("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Write word to port
static inline void outw(uint16_t port, uint16_t data) {
    __asm__ __volatile__("outw %0, %1" : : "a"(data), "Nd"(port));
}

// I/O wait (for slower devices)
static inline void io_wait(void) {
    // Port 0x80 is used for 'checkpoints' during POST
    // Writing to this port causes a small delay
    outb(0x80, 0);
}

#endif // PORTS_H
