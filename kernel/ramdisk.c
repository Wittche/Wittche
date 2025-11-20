// RAM Disk implementation - Virtual disk in memory
#include "../include/ramdisk.h"
#include "../include/string.h"
#include "../include/heap.h"

// Global RAM disk instance
static ramdisk_t ramdisk;

/**
 * Initialize RAM disk
 * Allocates memory and prepares the virtual disk
 */
void ramdisk_init(void) {
    // Allocate memory for RAM disk
    ramdisk.data = (uint8_t *)kmalloc(RAMDISK_SIZE);

    if (!ramdisk.data) {
        ramdisk.initialized = 0;
        return;
    }

    // Set configuration
    ramdisk.size = RAMDISK_SIZE;
    ramdisk.block_size = RAMDISK_BLOCK_SIZE;
    ramdisk.block_count = RAMDISK_BLOCK_COUNT;
    ramdisk.initialized = 1;

    // Clear the disk
    memset(ramdisk.data, 0, RAMDISK_SIZE);
}

/**
 * Read a block from RAM disk
 * block_num: Block number to read (0 to block_count-1)
 * buffer: Buffer to store the read data (must be at least block_size bytes)
 * Returns: 0 on success, -1 on error
 */
int ramdisk_read_block(uint32_t block_num, void *buffer) {
    // Check if initialized
    if (!ramdisk.initialized) {
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        return -1;
    }

    // Calculate block offset
    uint32_t offset = block_num * ramdisk.block_size;

    // Copy data from RAM disk to buffer
    memcpy(buffer, ramdisk.data + offset, ramdisk.block_size);

    return 0;
}

/**
 * Write a block to RAM disk
 * block_num: Block number to write (0 to block_count-1)
 * buffer: Data to write (must be at least block_size bytes)
 * Returns: 0 on success, -1 on error
 */
int ramdisk_write_block(uint32_t block_num, const void *buffer) {
    // Check if initialized
    if (!ramdisk.initialized) {
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        return -1;
    }

    // Calculate block offset
    uint32_t offset = block_num * ramdisk.block_size;

    // Copy data from buffer to RAM disk
    memcpy(ramdisk.data + offset, buffer, ramdisk.block_size);

    return 0;
}

/**
 * Format RAM disk (clear all data)
 * Fills entire disk with zeros
 */
void ramdisk_format(void) {
    if (!ramdisk.initialized) {
        return;
    }

    memset(ramdisk.data, 0, ramdisk.size);
}

/**
 * Get RAM disk info
 * Returns pointer to RAM disk structure
 */
ramdisk_t *ramdisk_get_info(void) {
    return &ramdisk;
}

/**
 * Check if RAM disk is initialized
 * Returns: 1 if initialized, 0 otherwise
 */
int ramdisk_is_initialized(void) {
    return ramdisk.initialized;
}
