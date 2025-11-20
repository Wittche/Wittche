// WitFS - Wittche File System
// Simple educational file system for RAM disk
#ifndef FS_H
#define FS_H

#include "types.h"

// File system magic number
#define FS_MAGIC 0x57495443  // 'WITC'
#define FS_VERSION 1

// File system layout (on 1MB RAM disk = 2048 blocks)
#define FS_BLOCK_SIZE 512
#define FS_SUPERBLOCK_BLOCK 0
#define FS_INODE_BITMAP_BLOCK 1
#define FS_DATA_BITMAP_BLOCK 2
#define FS_INODE_TABLE_BLOCK 3
#define FS_INODE_TABLE_BLOCKS 64   // 64 blocks for inode table
#define FS_DATA_BLOCKS_START 67     // Data blocks start at block 67

// File system limits
#define FS_MAX_INODES 512           // 64 blocks * 8 inodes per block
#define FS_MAX_DATA_BLOCKS 1981     // 2048 - 67 = 1981 data blocks
#define FS_MAX_FILENAME 27          // 27 chars + null terminator
#define FS_DIRECT_BLOCKS 10         // 10 direct blocks per inode (5KB max file)
#define FS_MAX_FILE_SIZE (FS_DIRECT_BLOCKS * FS_BLOCK_SIZE)

// File types
#define FS_TYPE_FILE 0x01
#define FS_TYPE_DIR 0x02

// File permissions (simple)
#define FS_PERM_READ 0x04
#define FS_PERM_WRITE 0x02
#define FS_PERM_EXEC 0x01
#define FS_PERM_DEFAULT (FS_PERM_READ | FS_PERM_WRITE)

// Max open files
#define FS_MAX_OPEN_FILES 16

/**
 * Superblock - File system metadata (512 bytes)
 * Located at block 0
 */
typedef struct {
    uint32_t magic;              // Must be FS_MAGIC
    uint32_t version;            // FS_VERSION
    uint32_t block_size;         // Block size (512)
    uint32_t total_blocks;       // Total blocks (2048)

    uint32_t inode_bitmap_block; // Inode bitmap location (block 1)
    uint32_t data_bitmap_block;  // Data bitmap location (block 2)
    uint32_t inode_table_block;  // Inode table location (block 3)
    uint32_t data_blocks_start;  // First data block (block 67)

    uint32_t total_inodes;       // Total inodes (512)
    uint32_t total_data_blocks;  // Total data blocks (1981)
    uint32_t free_inodes;        // Free inodes count
    uint32_t free_blocks;        // Free data blocks count

    uint32_t root_inode;         // Root directory inode (0)

    uint8_t reserved[460];       // Reserved for future use
} __attribute__((packed)) fs_superblock_t;

/**
 * Inode - File/Directory metadata (64 bytes)
 * 8 inodes per block
 */
typedef struct {
    uint32_t mode;           // Type and permissions
    uint32_t size;           // File size in bytes
    uint32_t uid;            // User ID (unused for now)
    uint32_t gid;            // Group ID (unused for now)

    uint32_t created;        // Creation timestamp (uptime ticks)
    uint32_t modified;       // Modification timestamp

    uint32_t blocks[FS_DIRECT_BLOCKS];  // Direct block pointers

    uint32_t reserved[4];    // Reserved
} __attribute__((packed)) fs_inode_t;

/**
 * Directory Entry (32 bytes)
 * 16 entries per block
 */
typedef struct {
    uint32_t inode;          // Inode number (0 = unused)
    char name[FS_MAX_FILENAME + 1];  // File/directory name
} __attribute__((packed)) fs_dirent_t;

/**
 * File Descriptor (for open files)
 */
typedef struct {
    uint32_t inode;          // Inode number
    uint32_t position;       // Current read/write position
    uint32_t flags;          // Open flags (read/write)
    uint8_t in_use;          // 1 if in use, 0 if free
} fs_file_t;

// File open flags
#define FS_OPEN_READ 0x01
#define FS_OPEN_WRITE 0x02
#define FS_OPEN_CREATE 0x04

/**
 * File System API
 */

// Initialize file system (mount)
void fs_init(void);

// Format file system (create new empty FS)
void fs_format(void);

// Check if file system is initialized
int fs_is_initialized(void);

// File operations
int fs_create(const char *path, uint32_t type);  // Create file or directory
int fs_unlink(const char *path);                 // Delete file or directory
int fs_open(const char *path, uint32_t flags);   // Open file, returns fd or -1
int fs_close(int fd);                            // Close file descriptor
int fs_read(int fd, void *buf, uint32_t size);   // Read from file
int fs_write(int fd, const void *buf, uint32_t size);  // Write to file

// Directory operations
int fs_readdir(const char *path, fs_dirent_t *entries, int max);  // List directory
int fs_mkdir(const char *path);                 // Create directory
int fs_rmdir(const char *path);                 // Remove directory

// Utility functions
int fs_stat(const char *path, fs_inode_t *stat);  // Get file info
int fs_exists(const char *path);                  // Check if file exists
void fs_get_stats(uint32_t *total_inodes, uint32_t *free_inodes,
                  uint32_t *total_blocks, uint32_t *free_blocks);

#endif // FS_H
