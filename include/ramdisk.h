// RAM Disk - Virtual disk in memory
#ifndef RAMDISK_H
#define RAMDISK_H

#include "types.h"

// RAM disk configuration
#define RAMDISK_SIZE (1024 * 1024)   // 1MB total size
#define RAMDISK_BLOCK_SIZE 512       // 512 bytes per block (standard)
#define RAMDISK_BLOCK_COUNT (RAMDISK_SIZE / RAMDISK_BLOCK_SIZE)  // 2048 blocks

/**
 * RAM disk structure
 */
typedef struct {
    uint8_t *data;           // Pointer to RAM disk data
    uint32_t size;           // Total size in bytes
    uint32_t block_size;     // Block size in bytes
    uint32_t block_count;    // Total number of blocks
    uint32_t initialized;    // 1 if initialized, 0 otherwise
} ramdisk_t;

/**
 * Initialize RAM disk
 * Allocates memory and prepares the virtual disk
 */
void ramdisk_init(void);

/**
 * Read a block from RAM disk
 * block_num: Block number to read (0 to block_count-1)
 * buffer: Buffer to store the read data (must be at least block_size bytes)
 * Returns: 0 on success, -1 on error
 */
int ramdisk_read_block(uint32_t block_num, void *buffer);

/**
 * Write a block to RAM disk
 * block_num: Block number to write (0 to block_count-1)
 * buffer: Data to write (must be at least block_size bytes)
 * Returns: 0 on success, -1 on error
 */
int ramdisk_write_block(uint32_t block_num, const void *buffer);

/**
 * Format RAM disk (clear all data)
 * Fills entire disk with zeros
 */
void ramdisk_format(void);

/**
 * Get RAM disk info
 * Returns pointer to RAM disk structure
 */
ramdisk_t *ramdisk_get_info(void);

/**
 * Check if RAM disk is initialized
 * Returns: 1 if initialized, 0 otherwise
 */
int ramdisk_is_initialized(void);

#endif // RAMDISK_H
