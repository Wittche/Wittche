// RAM Disk implementation - Virtual disk in memory
#include "../include/ramdisk.h"
#include "../include/string.h"
#include "../include/heap.h"
#include "../include/kprintf.h"

// Global RAM disk instance
static ramdisk_t ramdisk;

/**
 * Initialize RAM disk
 * Allocates memory and prepares the virtual disk
 */
void ramdisk_init(void) {
    kprintf("RAM Disk: Initializing...\n");
    kprintf("RAM Disk: DEBUG - About to allocate %d bytes\n", RAMDISK_SIZE);

    // Allocate memory for RAM disk
    ramdisk.data = (uint8_t *)kmalloc(RAMDISK_SIZE);

    kprintf("RAM Disk: DEBUG - kmalloc returned: 0x%X\n", (uint32_t)ramdisk.data);

    if (!ramdisk.data) {
        kprintf("RAM Disk: ERROR - Failed to allocate memory!\n");
        ramdisk.initialized = 0;
        kprintf("RAM Disk: DEBUG - initialized set to 0\n");
        return;
    }

    // Set configuration
    ramdisk.size = RAMDISK_SIZE;
    ramdisk.block_size = RAMDISK_BLOCK_SIZE;
    ramdisk.block_count = RAMDISK_BLOCK_COUNT;
    ramdisk.initialized = 1;

    kprintf("RAM Disk: DEBUG - initialized set to 1\n");

    // Clear the disk
    memset(ramdisk.data, 0, RAMDISK_SIZE);

    kprintf("RAM Disk: Initialized successfully\n");
    kprintf("  Size: %d bytes (%d KB)\n", RAMDISK_SIZE, RAMDISK_SIZE / 1024);
    kprintf("  Block Size: %d bytes\n", RAMDISK_BLOCK_SIZE);
    kprintf("  Block Count: %d blocks\n", RAMDISK_BLOCK_COUNT);
    kprintf("RAM Disk: DEBUG - Init complete, initialized = %d\n", ramdisk.initialized);
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
        kprintf("RAM Disk: ERROR - Not initialized!\n");
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        kprintf("RAM Disk: ERROR - Invalid block number %d (max %d)\n",
                block_num, ramdisk.block_count - 1);
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        kprintf("RAM Disk: ERROR - NULL buffer!\n");
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
        kprintf("RAM Disk: ERROR - Not initialized!\n");
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        kprintf("RAM Disk: ERROR - Invalid block number %d (max %d)\n",
                block_num, ramdisk.block_count - 1);
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        kprintf("RAM Disk: ERROR - NULL buffer!\n");
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
        kprintf("RAM Disk: ERROR - Not initialized!\n");
        return;
    }

    kprintf("RAM Disk: Formatting...\n");
    memset(ramdisk.data, 0, ramdisk.size);
    kprintf("RAM Disk: Format complete\n");
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
