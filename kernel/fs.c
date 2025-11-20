// WitFS - Wittche File System Implementation
#include "../include/fs.h"
#include "../include/ramdisk.h"
#include "../include/string.h"
#include "../include/heap.h"
#include "../include/timer.h"
#include "../include/screen.h"

// Global file system state
static fs_superblock_t superblock;
static uint8_t *inode_bitmap = NULL;
static uint8_t *data_bitmap = NULL;
static fs_file_t open_files[FS_MAX_OPEN_FILES];
static uint8_t fs_initialized = 0;

// Helper functions
static uint32_t bitmap_find_free(uint8_t *bitmap, uint32_t max_bits);
static void bitmap_set(uint8_t *bitmap, uint32_t bit);
static void bitmap_clear(uint8_t *bitmap, uint32_t bit);
static int bitmap_test(uint8_t *bitmap, uint32_t bit);
static int read_inode(uint32_t inode_num, fs_inode_t *inode);
static int write_inode(uint32_t inode_num, const fs_inode_t *inode);
static uint32_t alloc_inode(void);
static void free_inode(uint32_t inode_num);
static uint32_t alloc_data_block(void);
static void free_data_block(uint32_t block_num);
static int path_lookup(const char *path, uint32_t *inode_num);
static int dir_add_entry(uint32_t dir_inode, const char *name, uint32_t inode);
static int dir_remove_entry(uint32_t dir_inode, const char *name);

/**
 * Initialize file system (mount)
 */
void fs_init(void) {
    screen_write_color("[FS_INIT] Starting file system mount...\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Check if RAM disk is initialized
    if (!ramdisk_is_initialized()) {
        screen_write_color("[FS_INIT] ERROR: RAM disk not initialized!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return;
    }
    screen_write_color("[FS_INIT] RAM disk is initialized\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Allocate bitmaps in kernel heap
    inode_bitmap = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    data_bitmap = (uint8_t *)kmalloc(FS_BLOCK_SIZE);

    if (!inode_bitmap || !data_bitmap) {
        screen_write_color("[FS_INIT] ERROR: Failed to allocate bitmaps!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        if (inode_bitmap) kfree(inode_bitmap);
        if (data_bitmap) kfree(data_bitmap);
        return;
    }
    screen_write_color("[FS_INIT] Bitmaps allocated\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Read superblock from disk
    uint8_t *temp_block = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!temp_block) {
        screen_write_color("[FS_INIT] ERROR: Failed to allocate temp block!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        kfree(inode_bitmap);
        kfree(data_bitmap);
        return;
    }

    ramdisk_read_block(FS_SUPERBLOCK_BLOCK, temp_block);
    memcpy(&superblock, temp_block, sizeof(fs_superblock_t));
    kfree(temp_block);

    screen_write_color("[FS_INIT] Superblock read, magic=0x", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));
    screen_write_hex(superblock.magic);
    screen_write("\n");

    // Check magic number
    if (superblock.magic != FS_MAGIC) {
        screen_write_color("[FS_INIT] ERROR: Invalid magic number! Expected 0x", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        screen_write_hex(FS_MAGIC);
        screen_write(", got 0x");
        screen_write_hex(superblock.magic);
        screen_write("\n");
        kfree(inode_bitmap);
        kfree(data_bitmap);
        return;
    }

    // Load bitmaps from disk
    ramdisk_read_block(FS_INODE_BITMAP_BLOCK, inode_bitmap);
    ramdisk_read_block(FS_DATA_BITMAP_BLOCK, data_bitmap);
    screen_write_color("[FS_INIT] Bitmaps loaded from disk\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Initialize open files table
    for (int i = 0; i < FS_MAX_OPEN_FILES; i++) {
        open_files[i].in_use = 0;
    }

    fs_initialized = 1;
    screen_write_color("[FS_INIT] File system mounted successfully!\n", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
}

/**
 * Format file system (create new empty FS)
 */
void fs_format(void) {
    screen_write_color("[FS_FORMAT] Starting file system format...\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Check if RAM disk is initialized
    if (!ramdisk_is_initialized()) {
        screen_write_color("[FS_FORMAT] ERROR: RAM disk not initialized!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return;
    }

    // Allocate bitmaps
    if (!inode_bitmap) {
        inode_bitmap = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    }
    if (!data_bitmap) {
        data_bitmap = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    }

    if (!inode_bitmap || !data_bitmap) {
        screen_write_color("[FS_FORMAT] ERROR: Failed to allocate bitmaps!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return;
    }
    screen_write_color("[FS_FORMAT] Bitmaps allocated\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    // Initialize superblock
    memset(&superblock, 0, sizeof(fs_superblock_t));
    superblock.magic = FS_MAGIC;
    superblock.version = FS_VERSION;
    superblock.block_size = FS_BLOCK_SIZE;
    superblock.total_blocks = 2048;
    superblock.inode_bitmap_block = FS_INODE_BITMAP_BLOCK;
    superblock.data_bitmap_block = FS_DATA_BITMAP_BLOCK;
    superblock.inode_table_block = FS_INODE_TABLE_BLOCK;
    superblock.data_blocks_start = FS_DATA_BLOCKS_START;
    superblock.total_inodes = FS_MAX_INODES;
    superblock.total_data_blocks = FS_MAX_DATA_BLOCKS;
    superblock.free_inodes = FS_MAX_INODES - 1;  // -1 for root
    superblock.free_blocks = FS_MAX_DATA_BLOCKS - 1;  // -1 for root dir data
    superblock.root_inode = 0;

    // Clear bitmaps
    memset(inode_bitmap, 0, FS_BLOCK_SIZE);
    memset(data_bitmap, 0, FS_BLOCK_SIZE);

    // Allocate root inode (inode 0)
    bitmap_set(inode_bitmap, 0);

    // Allocate root directory data block (block 0)
    bitmap_set(data_bitmap, 0);

    // Create root directory inode
    fs_inode_t root_inode;
    memset(&root_inode, 0, sizeof(fs_inode_t));
    root_inode.mode = FS_TYPE_DIR | FS_PERM_DEFAULT;
    root_inode.size = 0;
    root_inode.created = timer_get_ticks();
    root_inode.modified = root_inode.created;
    root_inode.blocks[0] = 0;  // First data block

    write_inode(0, &root_inode);

    // Clear root directory data block
    uint8_t *empty_block = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (empty_block) {
        memset(empty_block, 0, FS_BLOCK_SIZE);
        ramdisk_write_block(FS_DATA_BLOCKS_START, empty_block);
        kfree(empty_block);
    }

    // Write superblock to disk
    uint8_t *temp_block = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (temp_block) {
        memset(temp_block, 0, FS_BLOCK_SIZE);
        memcpy(temp_block, &superblock, sizeof(fs_superblock_t));
        ramdisk_write_block(FS_SUPERBLOCK_BLOCK, temp_block);
        kfree(temp_block);
    }

    // Write bitmaps to disk
    ramdisk_write_block(FS_INODE_BITMAP_BLOCK, inode_bitmap);
    ramdisk_write_block(FS_DATA_BITMAP_BLOCK, data_bitmap);
    screen_write_color("[FS_FORMAT] Bitmaps written to disk\n", MAKE_COLOR(COLOR_CYAN, COLOR_BLACK));

    fs_initialized = 1;
    screen_write_color("[FS_FORMAT] File system formatted successfully!\n", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
}

/**
 * Check if file system is initialized
 */
int fs_is_initialized(void) {
    return fs_initialized;
}

/**
 * Bitmap operations
 */
static uint32_t bitmap_find_free(uint8_t *bitmap, uint32_t max_bits) {
    for (uint32_t i = 0; i < max_bits; i++) {
        if (!bitmap_test(bitmap, i)) {
            return i;
        }
    }
    return (uint32_t)-1;
}

static void bitmap_set(uint8_t *bitmap, uint32_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear(uint8_t *bitmap, uint32_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test(uint8_t *bitmap, uint32_t bit) {
    return (bitmap[bit / 8] & (1 << (bit % 8))) != 0;
}

/**
 * Inode operations
 */
static int read_inode(uint32_t inode_num, fs_inode_t *inode) {
    if (inode_num >= FS_MAX_INODES) {
        return -1;
    }

    // Calculate block and offset
    uint32_t inodes_per_block = FS_BLOCK_SIZE / sizeof(fs_inode_t);
    uint32_t block = FS_INODE_TABLE_BLOCK + (inode_num / inodes_per_block);
    uint32_t offset = (inode_num % inodes_per_block) * sizeof(fs_inode_t);

    // Read block
    uint8_t *temp = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!temp) return -1;

    ramdisk_read_block(block, temp);
    memcpy(inode, temp + offset, sizeof(fs_inode_t));
    kfree(temp);

    return 0;
}

static int write_inode(uint32_t inode_num, const fs_inode_t *inode) {
    if (inode_num >= FS_MAX_INODES) {
        return -1;
    }

    // Calculate block and offset
    uint32_t inodes_per_block = FS_BLOCK_SIZE / sizeof(fs_inode_t);
    uint32_t block = FS_INODE_TABLE_BLOCK + (inode_num / inodes_per_block);
    uint32_t offset = (inode_num % inodes_per_block) * sizeof(fs_inode_t);

    // Read block, modify, write back
    uint8_t *temp = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!temp) return -1;

    ramdisk_read_block(block, temp);
    memcpy(temp + offset, inode, sizeof(fs_inode_t));
    ramdisk_write_block(block, temp);
    kfree(temp);

    return 0;
}

static uint32_t alloc_inode(void) {
    uint32_t inode = bitmap_find_free(inode_bitmap, FS_MAX_INODES);
    if (inode != (uint32_t)-1) {
        bitmap_set(inode_bitmap, inode);
        superblock.free_inodes--;
        ramdisk_write_block(FS_INODE_BITMAP_BLOCK, inode_bitmap);
    }
    return inode;
}

static void free_inode(uint32_t inode_num) {
    bitmap_clear(inode_bitmap, inode_num);
    superblock.free_inodes++;
    ramdisk_write_block(FS_INODE_BITMAP_BLOCK, inode_bitmap);
}

static uint32_t alloc_data_block(void) {
    uint32_t block = bitmap_find_free(data_bitmap, FS_MAX_DATA_BLOCKS);
    if (block != (uint32_t)-1) {
        bitmap_set(data_bitmap, block);
        superblock.free_blocks--;
        ramdisk_write_block(FS_DATA_BITMAP_BLOCK, data_bitmap);
    }
    return block;
}

static void free_data_block(uint32_t block_num) {
    bitmap_clear(data_bitmap, block_num);
    superblock.free_blocks++;
    ramdisk_write_block(FS_DATA_BITMAP_BLOCK, data_bitmap);
}

/**
 * Path lookup - find inode number for a path
 * For now, only support files in root directory (no subdirectories)
 * Path format: "/filename" or "filename"
 */
static int path_lookup(const char *path, uint32_t *inode_num) {
    if (!path) return -1;

    // Skip leading slash
    const char *name = path;
    if (*name == '/') name++;

    // If empty, return root inode
    if (*name == '\0') {
        *inode_num = 0;
        return 0;
    }

    // For now, only support root directory files
    // Look up name in root directory
    fs_inode_t root;
    if (read_inode(0, &root) < 0) {
        return -1;
    }

    // Root must be a directory
    if ((root.mode & FS_TYPE_DIR) == 0) {
        return -1;
    }

    // Read root directory entries
    uint8_t *data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!data) return -1;

    for (int block_idx = 0; block_idx < FS_DIRECT_BLOCKS && root.blocks[block_idx] != 0; block_idx++) {
        uint32_t data_block = FS_DATA_BLOCKS_START + root.blocks[block_idx];
        ramdisk_read_block(data_block, data);

        fs_dirent_t *entries = (fs_dirent_t *)data;
        int entries_per_block = FS_BLOCK_SIZE / sizeof(fs_dirent_t);

        for (int i = 0; i < entries_per_block; i++) {
            if (entries[i].inode != 0 && strcmp(entries[i].name, name) == 0) {
                *inode_num = entries[i].inode;
                kfree(data);
                return 0;
            }
        }
    }

    kfree(data);
    return -1;  // Not found
}

/**
 * Add entry to directory
 */
static int dir_add_entry(uint32_t dir_inode_num, const char *name, uint32_t inode) {
    fs_inode_t dir_inode;
    if (read_inode(dir_inode_num, &dir_inode) < 0) {
        return -1;
    }

    // Must be a directory
    if ((dir_inode.mode & FS_TYPE_DIR) == 0) {
        return -1;
    }

    // Find free entry
    uint8_t *data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!data) return -1;

    int entries_per_block = FS_BLOCK_SIZE / sizeof(fs_dirent_t);

    for (int block_idx = 0; block_idx < FS_DIRECT_BLOCKS; block_idx++) {
        // If block doesn't exist, allocate it
        if (dir_inode.blocks[block_idx] == 0) {
            uint32_t new_block = alloc_data_block();
            if (new_block == (uint32_t)-1) {
                kfree(data);
                return -1;
            }
            dir_inode.blocks[block_idx] = new_block;
            memset(data, 0, FS_BLOCK_SIZE);
        } else {
            uint32_t data_block = FS_DATA_BLOCKS_START + dir_inode.blocks[block_idx];
            ramdisk_read_block(data_block, data);
        }

        fs_dirent_t *entries = (fs_dirent_t *)data;

        // Find free slot
        for (int i = 0; i < entries_per_block; i++) {
            if (entries[i].inode == 0) {
                entries[i].inode = inode;
                strncpy(entries[i].name, name, FS_MAX_FILENAME);
                entries[i].name[FS_MAX_FILENAME] = '\0';

                uint32_t data_block = FS_DATA_BLOCKS_START + dir_inode.blocks[block_idx];
                ramdisk_write_block(data_block, data);

                dir_inode.size += sizeof(fs_dirent_t);
                dir_inode.modified = timer_get_ticks();
                write_inode(dir_inode_num, &dir_inode);

                kfree(data);
                return 0;
            }
        }
    }

    kfree(data);
    return -1;  // Directory full
}

/**
 * Remove entry from directory
 */
static int dir_remove_entry(uint32_t dir_inode_num, const char *name) {
    fs_inode_t dir_inode;
    if (read_inode(dir_inode_num, &dir_inode) < 0) {
        return -1;
    }

    uint8_t *data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!data) return -1;

    for (int block_idx = 0; block_idx < FS_DIRECT_BLOCKS && dir_inode.blocks[block_idx] != 0; block_idx++) {
        uint32_t data_block = FS_DATA_BLOCKS_START + dir_inode.blocks[block_idx];
        ramdisk_read_block(data_block, data);

        fs_dirent_t *entries = (fs_dirent_t *)data;
        int entries_per_block = FS_BLOCK_SIZE / sizeof(fs_dirent_t);

        for (int i = 0; i < entries_per_block; i++) {
            if (entries[i].inode != 0 && strcmp(entries[i].name, name) == 0) {
                entries[i].inode = 0;
                ramdisk_write_block(data_block, data);

                dir_inode.size -= sizeof(fs_dirent_t);
                dir_inode.modified = timer_get_ticks();
                write_inode(dir_inode_num, &dir_inode);

                kfree(data);
                return 0;
            }
        }
    }

    kfree(data);
    return -1;  // Not found
}

/**
 * Create file or directory
 */
int fs_create(const char *path, uint32_t type) {
    if (!fs_initialized) return -1;
    if (!path) return -1;

    // Extract filename
    const char *name = path;
    if (*name == '/') name++;

    // Check if already exists
    uint32_t existing;
    if (path_lookup(path, &existing) == 0) {
        return -1;  // Already exists
    }

    // Allocate inode
    uint32_t inode_num = alloc_inode();
    if (inode_num == (uint32_t)-1) {
        return -1;
    }

    // Create inode
    fs_inode_t inode;
    memset(&inode, 0, sizeof(fs_inode_t));
    inode.mode = type | FS_PERM_DEFAULT;
    inode.size = 0;
    inode.created = timer_get_ticks();
    inode.modified = inode.created;

    // If directory, allocate data block
    if (type == FS_TYPE_DIR) {
        uint32_t block = alloc_data_block();
        if (block == (uint32_t)-1) {
            free_inode(inode_num);
            return -1;
        }
        inode.blocks[0] = block;

        // Clear directory block
        uint8_t *data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
        if (data) {
            memset(data, 0, FS_BLOCK_SIZE);
            ramdisk_write_block(FS_DATA_BLOCKS_START + block, data);
            kfree(data);
        }
    }

    write_inode(inode_num, &inode);

    // Add to root directory
    if (dir_add_entry(0, name, inode_num) < 0) {
        free_inode(inode_num);
        if (type == FS_TYPE_DIR && inode.blocks[0] != 0) {
            free_data_block(inode.blocks[0]);
        }
        return -1;
    }

    return 0;
}

/**
 * Delete file or directory
 */
int fs_unlink(const char *path) {
    if (!fs_initialized) return -1;

    // Look up inode
    uint32_t inode_num;
    if (path_lookup(path, &inode_num) < 0) {
        return -1;  // Not found
    }

    // Read inode
    fs_inode_t inode;
    if (read_inode(inode_num, &inode) < 0) {
        return -1;
    }

    // Free data blocks
    for (int i = 0; i < FS_DIRECT_BLOCKS && inode.blocks[i] != 0; i++) {
        free_data_block(inode.blocks[i]);
    }

    // Free inode
    free_inode(inode_num);

    // Remove from directory
    const char *name = path;
    if (*name == '/') name++;
    dir_remove_entry(0, name);

    return 0;
}

/**
 * Open file
 */
int fs_open(const char *path, uint32_t flags) {
    screen_write_color("[FS_OPEN] Opening file: ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write(path);
    screen_write(", flags=0x");
    screen_write_hex(flags);
    screen_write("\n");

    if (!fs_initialized) {
        screen_write_color("[FS_OPEN] ERROR: File system not initialized!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;
    }

    // Look up inode
    uint32_t inode_num;
    if (path_lookup(path, &inode_num) < 0) {
        screen_write_color("[FS_OPEN] File not found\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        // If CREATE flag, create it
        if (flags & FS_OPEN_CREATE) {
            screen_write_color("[FS_OPEN] Creating file...\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
            if (fs_create(path, FS_TYPE_FILE) < 0) {
                screen_write_color("[FS_OPEN] ERROR: Failed to create file!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
                return -1;
            }
            if (path_lookup(path, &inode_num) < 0) {
                screen_write_color("[FS_OPEN] ERROR: Created file but can't find it!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
                return -1;
            }
            screen_write_color("[FS_OPEN] File created, inode=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
            screen_write_dec(inode_num);
            screen_write("\n");
        } else {
            screen_write_color("[FS_OPEN] ERROR: File not found and CREATE flag not set!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
            return -1;  // Not found
        }
    } else {
        screen_write_color("[FS_OPEN] File found, inode=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write_dec(inode_num);
        screen_write("\n");
    }

    // Find free file descriptor
    int fd = -1;
    for (int i = 0; i < FS_MAX_OPEN_FILES; i++) {
        if (!open_files[i].in_use) {
            fd = i;
            break;
        }
    }

    if (fd < 0) {
        screen_write_color("[FS_OPEN] ERROR: Too many open files!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;  // Too many open files
    }

    // Initialize file descriptor
    open_files[fd].inode = inode_num;
    open_files[fd].position = 0;
    open_files[fd].flags = flags;
    open_files[fd].in_use = 1;

    screen_write_color("[FS_OPEN] File opened successfully, fd=", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write_dec(fd);
    screen_write("\n");

    return fd;
}

/**
 * Close file
 */
int fs_close(int fd) {
    if (fd < 0 || fd >= FS_MAX_OPEN_FILES) {
        return -1;
    }

    if (!open_files[fd].in_use) {
        return -1;
    }

    open_files[fd].in_use = 0;
    return 0;
}

/**
 * Read from file
 */
int fs_read(int fd, void *buf, uint32_t size) {
    screen_write_color("[FS_READ] Reading from fd=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_dec(fd);
    screen_write(", size=");
    screen_write_dec(size);
    screen_write(" bytes\n");

    if (fd < 0 || fd >= FS_MAX_OPEN_FILES || !open_files[fd].in_use) {
        screen_write_color("[FS_READ] ERROR: Invalid file descriptor!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;
    }

    if (!(open_files[fd].flags & FS_OPEN_READ)) {
        screen_write_color("[FS_READ] ERROR: File not open for reading!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;  // Not open for reading
    }

    // Read inode
    fs_inode_t inode;
    if (read_inode(open_files[fd].inode, &inode) < 0) {
        screen_write_color("[FS_READ] ERROR: Failed to read inode!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;
    }

    screen_write_color("[FS_READ] Inode read, inode=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_dec(open_files[fd].inode);
    screen_write(", file size=");
    screen_write_dec(inode.size);
    screen_write(" bytes\n");

    // Can't read past end of file
    if (open_files[fd].position >= inode.size) {
        screen_write_color("[FS_READ] At EOF, returning 0\n", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        return 0;  // EOF
    }

    // Limit read to file size
    if (open_files[fd].position + size > inode.size) {
        size = inode.size - open_files[fd].position;
        screen_write_color("[FS_READ] Limiting read to ", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
        screen_write_dec(size);
        screen_write(" bytes\n");
    }

    uint32_t bytes_read = 0;
    uint8_t *dest = (uint8_t *)buf;
    uint8_t *block_data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!block_data) return -1;

    while (bytes_read < size) {
        uint32_t block_idx = open_files[fd].position / FS_BLOCK_SIZE;
        uint32_t block_offset = open_files[fd].position % FS_BLOCK_SIZE;
        uint32_t block_remaining = FS_BLOCK_SIZE - block_offset;
        uint32_t to_read = (size - bytes_read < block_remaining) ? (size - bytes_read) : block_remaining;

        if (block_idx >= FS_DIRECT_BLOCKS || inode.blocks[block_idx] == 0) {
            break;
        }

        uint32_t data_block = FS_DATA_BLOCKS_START + inode.blocks[block_idx];
        ramdisk_read_block(data_block, block_data);
        memcpy(dest + bytes_read, block_data + block_offset, to_read);

        bytes_read += to_read;
        open_files[fd].position += to_read;
    }

    kfree(block_data);

    screen_write_color("[FS_READ] Read completed, bytes_read=", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write_dec(bytes_read);
    screen_write("\n");

    return bytes_read;
}

/**
 * Write to file
 */
int fs_write(int fd, const void *buf, uint32_t size) {
    screen_write_color("[FS_WRITE] Writing to fd=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_dec(fd);
    screen_write(", size=");
    screen_write_dec(size);
    screen_write(" bytes\n");

    if (fd < 0 || fd >= FS_MAX_OPEN_FILES || !open_files[fd].in_use) {
        screen_write_color("[FS_WRITE] ERROR: Invalid file descriptor!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;
    }

    if (!(open_files[fd].flags & FS_OPEN_WRITE)) {
        screen_write_color("[FS_WRITE] ERROR: File not open for writing!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;  // Not open for writing
    }

    // Read inode
    fs_inode_t inode;
    if (read_inode(open_files[fd].inode, &inode) < 0) {
        screen_write_color("[FS_WRITE] ERROR: Failed to read inode!\n", MAKE_COLOR(COLOR_RED, COLOR_BLACK));
        return -1;
    }
    screen_write_color("[FS_WRITE] Inode read successfully, inode=", MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK));
    screen_write_dec(open_files[fd].inode);
    screen_write("\n");

    uint32_t bytes_written = 0;
    const uint8_t *src = (const uint8_t *)buf;
    uint8_t *block_data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!block_data) return -1;

    while (bytes_written < size) {
        uint32_t block_idx = open_files[fd].position / FS_BLOCK_SIZE;
        uint32_t block_offset = open_files[fd].position % FS_BLOCK_SIZE;
        uint32_t block_remaining = FS_BLOCK_SIZE - block_offset;
        uint32_t to_write = (size - bytes_written < block_remaining) ? (size - bytes_written) : block_remaining;

        if (block_idx >= FS_DIRECT_BLOCKS) {
            break;  // File too large
        }

        // Allocate block if needed
        if (inode.blocks[block_idx] == 0) {
            uint32_t new_block = alloc_data_block();
            if (new_block == (uint32_t)-1) {
                break;  // Out of space
            }
            inode.blocks[block_idx] = new_block;
            memset(block_data, 0, FS_BLOCK_SIZE);
        } else {
            uint32_t data_block = FS_DATA_BLOCKS_START + inode.blocks[block_idx];
            ramdisk_read_block(data_block, block_data);
        }

        memcpy(block_data + block_offset, src + bytes_written, to_write);

        uint32_t data_block = FS_DATA_BLOCKS_START + inode.blocks[block_idx];
        ramdisk_write_block(data_block, block_data);

        bytes_written += to_write;
        open_files[fd].position += to_write;

        // Update file size
        if (open_files[fd].position > inode.size) {
            inode.size = open_files[fd].position;
        }
    }

    kfree(block_data);

    // Update inode
    inode.modified = timer_get_ticks();
    write_inode(open_files[fd].inode, &inode);

    screen_write_color("[FS_WRITE] Write completed, bytes_written=", MAKE_COLOR(COLOR_GREEN, COLOR_BLACK));
    screen_write_dec(bytes_written);
    screen_write(", new file size=");
    screen_write_dec(inode.size);
    screen_write("\n");

    return bytes_written;
}

/**
 * Read directory entries
 */
int fs_readdir(const char *path, fs_dirent_t *entries, int max) {
    if (!fs_initialized) return -1;

    // Look up directory inode
    uint32_t inode_num;
    if (path_lookup(path, &inode_num) < 0) {
        return -1;
    }

    fs_inode_t inode;
    if (read_inode(inode_num, &inode) < 0) {
        return -1;
    }

    // Must be directory
    if ((inode.mode & FS_TYPE_DIR) == 0) {
        return -1;
    }

    uint8_t *data = (uint8_t *)kmalloc(FS_BLOCK_SIZE);
    if (!data) return -1;

    int count = 0;
    int entries_per_block = FS_BLOCK_SIZE / sizeof(fs_dirent_t);

    for (int block_idx = 0; block_idx < FS_DIRECT_BLOCKS && inode.blocks[block_idx] != 0 && count < max; block_idx++) {
        uint32_t data_block = FS_DATA_BLOCKS_START + inode.blocks[block_idx];
        ramdisk_read_block(data_block, data);

        fs_dirent_t *dir_entries = (fs_dirent_t *)data;

        for (int i = 0; i < entries_per_block && count < max; i++) {
            if (dir_entries[i].inode != 0) {
                memcpy(&entries[count], &dir_entries[i], sizeof(fs_dirent_t));
                count++;
            }
        }
    }

    kfree(data);
    return count;
}

/**
 * Create directory
 */
int fs_mkdir(const char *path) {
    return fs_create(path, FS_TYPE_DIR);
}

/**
 * Remove directory
 */
int fs_rmdir(const char *path) {
    return fs_unlink(path);
}

/**
 * Get file statistics
 */
int fs_stat(const char *path, fs_inode_t *stat) {
    if (!fs_initialized) return -1;

    uint32_t inode_num;
    if (path_lookup(path, &inode_num) < 0) {
        return -1;
    }

    return read_inode(inode_num, stat);
}

/**
 * Check if file exists
 */
int fs_exists(const char *path) {
    uint32_t inode_num;
    return path_lookup(path, &inode_num) == 0 ? 1 : 0;
}

/**
 * Get file system statistics
 */
void fs_get_stats(uint32_t *total_inodes, uint32_t *free_inodes,
                  uint32_t *total_blocks, uint32_t *free_blocks) {
    if (total_inodes) *total_inodes = superblock.total_inodes;
    if (free_inodes) *free_inodes = superblock.free_inodes;
    if (total_blocks) *total_blocks = superblock.total_data_blocks;
    if (free_blocks) *free_blocks = superblock.free_blocks;
}

