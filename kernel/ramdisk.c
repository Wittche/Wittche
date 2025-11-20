// RAM Disk implementation - Virtual disk in memory
#include "../include/ramdisk.h"
#include "../include/string.h"
#include "../include/heap.h"
#include "../include/screen.h"

// Global RAM disk instance
static ramdisk_t ramdisk;

/**
 * Initialize RAM disk
 * Allocates memory and prepares the virtual disk
 */
void ramdisk_init(void) {
    screen_write("=== RAMDISK INIT START ===\n");
    screen_write("About to allocate memory\n");

    // Allocate memory for RAM disk
    ramdisk.data = (uint8_t *)kmalloc(RAMDISK_SIZE);

    screen_write("kmalloc returned: 0x");
    screen_write_hex((uint32_t)ramdisk.data);
    screen_write("\n");

    if (!ramdisk.data) {
        screen_write("ERROR: kmalloc FAILED!\n");
        ramdisk.initialized = 0;
        return;
    }

    // Set configuration
    ramdisk.size = RAMDISK_SIZE;
    ramdisk.block_size = RAMDISK_BLOCK_SIZE;
    ramdisk.block_count = RAMDISK_BLOCK_COUNT;
    ramdisk.initialized = 1;

    screen_write("Set initialized to: ");
    screen_write_dec(ramdisk.initialized);
    screen_write("\n");

    // Clear the disk
    memset(ramdisk.data, 0, RAMDISK_SIZE);

    screen_write("RAM Disk: SUCCESS!\n");
    screen_write("=== RAMDISK INIT COMPLETE ===\n");
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
        screen_write("RAM Disk: ERROR - Not initialized!\n");
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        screen_write("RAM Disk: ERROR - Invalid block number ");
        screen_write_dec(block_num);
        screen_write(" (max ");
        screen_write_dec(ramdisk.block_count - 1);
        screen_write(")\n");
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        screen_write("RAM Disk: ERROR - NULL buffer!\n");
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
        screen_write("RAM Disk: ERROR - Not initialized!\n");
        return -1;
    }

    // Validate block number
    if (block_num >= ramdisk.block_count) {
        screen_write("RAM Disk: ERROR - Invalid block number ");
        screen_write_dec(block_num);
        screen_write(" (max ");
        screen_write_dec(ramdisk.block_count - 1);
        screen_write(")\n");
        return -1;
    }

    // Validate buffer
    if (!buffer) {
        screen_write("RAM Disk: ERROR - NULL buffer!\n");
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
        screen_write("RAM Disk: ERROR - Not initialized!\n");
        return;
    }

    screen_write("RAM Disk: Formatting...\n");
    memset(ramdisk.data, 0, ramdisk.size);
    screen_write("RAM Disk: Format complete\n");
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
    screen_write("[ramdisk_is_initialized] Checking: ramdisk.initialized = ");
    screen_write_dec(ramdisk.initialized);
    screen_write("\n");
    return ramdisk.initialized;
}
