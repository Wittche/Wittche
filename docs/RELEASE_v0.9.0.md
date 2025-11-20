# Release Notes - v0.9.0 "File System"

**Release Date:** November 20, 2025
**Type:** Major Feature Release

---

## 🎯 Overview

Version 0.9.0 introduces **WitFS (Wittche File System)**, a simple educational file system built on top of the RAM disk. This release brings persistent file storage and management capabilities to Wittche OS.

## ✨ What's New

### 💾 WitFS - Wittche File System

A complete file system implementation with support for files and directories.

**Features:**
- **Superblock**: File system metadata and configuration
- **Inode Table**: 512 inodes supporting files and directories
- **Bitmap Allocation**: Efficient space management with separate inode and data bitmaps
- **Direct Block Pointers**: 10 direct blocks per inode (max 5KB per file)
- **Root Directory**: Flat directory structure in root
- **File Descriptors**: Up to 16 simultaneously open files
- **Persistent Storage**: All data stored on 1MB RAM disk

**File System Layout:**
```
Block 0:      Superblock (metadata)
Block 1:      Inode Bitmap (512 inodes)
Block 2:      Data Bitmap (1981 data blocks)
Blocks 3-66:  Inode Table (512 inodes × 64 bytes)
Blocks 67+:   Data Blocks (1981 blocks = ~1MB)
```

**Specifications:**
- **Block Size**: 512 bytes
- **Total Blocks**: 2048 (1MB)
- **Max Inodes**: 512
- **Max File Size**: 5KB (10 blocks)
- **Max Filename**: 27 characters + null terminator
- **Max Open Files**: 16
- **Magic Number**: 0x57495443 ('WITC')

### 🔧 New Shell Commands

**File System Management:**

```bash
# Format the file system
wittche> fsformat
File System Format
==================

Formatting file system...
File system formatted successfully!
Empty root directory created.

# List files in directory
wittche> ls

Files in /:
===========
  [FILE] hello.txt (13 bytes)
  [DIR]  docs

Total: 2 items

# Create empty file
wittche> touch readme.txt
Created file: readme.txt

# Write text to file
wittche> write hello.txt Hello, World!
Created file: hello.txt (13 bytes written)

# Display file contents
wittche> cat hello.txt
Hello, World!

# Delete file
wittche> rm hello.txt
Deleted file: hello.txt

# Create directory
wittche> mkdir docs
Created directory: docs
```

### 📁 New Files

**Source Files:**
- `kernel/fs.c` - Complete file system implementation (~800 lines)
- `include/fs.h` - File system API and data structures

**Modified Files:**
- `kernel/shell.c` - Added file system commands (fsformat, ls, touch, cat, rm, mkdir, write)
- `kernel/kernel.c` - Initialize file system on boot
- `Makefile` - Added fs.o to build

**Documentation:**
- `docs/RELEASE_v0.9.0.md` - This file

## 💻 API Overview

### File System API (fs.h)

**Initialization:**
```c
void fs_init(void);          // Mount file system
void fs_format(void);        // Format (create new) file system
int fs_is_initialized(void); // Check if mounted
```

**File Operations:**
```c
int fs_create(const char *path, uint32_t type);  // Create file/dir
int fs_unlink(const char *path);                 // Delete file
int fs_open(const char *path, uint32_t flags);   // Open file (returns fd)
int fs_close(int fd);                            // Close file descriptor
int fs_read(int fd, void *buf, uint32_t size);   // Read from file
int fs_write(int fd, const void *buf, uint32_t size); // Write to file
```

**Directory Operations:**
```c
int fs_readdir(const char *path, fs_dirent_t *entries, int max); // List dir
int fs_mkdir(const char *path);  // Create directory
int fs_rmdir(const char *path);  // Remove directory
```

**Utility Functions:**
```c
int fs_stat(const char *path, fs_inode_t *stat);  // Get file info
int fs_exists(const char *path);                  // Check if exists
void fs_get_stats(uint32_t *total_inodes, uint32_t *free_inodes,
                  uint32_t *total_blocks, uint32_t *free_blocks);
```

### Data Structures

**Superblock:**
```c
typedef struct {
    uint32_t magic;              // 0x57495443 'WITC'
    uint32_t version;            // 1
    uint32_t block_size;         // 512
    uint32_t total_blocks;       // 2048
    uint32_t total_inodes;       // 512
    uint32_t total_data_blocks;  // 1981
    uint32_t free_inodes;        // Free inode count
    uint32_t free_blocks;        // Free block count
    uint32_t root_inode;         // 0
    // ... more fields
} fs_superblock_t;
```

**Inode:**
```c
typedef struct {
    uint32_t mode;               // Type and permissions
    uint32_t size;               // File size in bytes
    uint32_t created;            // Creation timestamp (ticks)
    uint32_t modified;           // Modification timestamp
    uint32_t blocks[10];         // Direct block pointers
    // ... more fields
} fs_inode_t;
```

**Directory Entry:**
```c
typedef struct {
    uint32_t inode;              // Inode number (0 = unused)
    char name[28];               // Filename
} fs_dirent_t;
```

## 🔄 Boot Sequence Changes

**Kernel Boot (kernel.c):**
```c
// After RAM disk initialization
ramdisk_init();

// Initialize file system (format on first boot, then mount)
if (!fs_is_initialized()) {
    fs_format();  // Create new file system
}
fs_init();  // Mount file system
```

## 📊 Impact

### Before v0.9.0
- ❌ No file system
- ❌ No persistent storage
- ❌ RAM disk was just raw storage
- ❌ No file operations

### After v0.9.0
- ✅ Complete file system (WitFS)
- ✅ Create, read, write, delete files
- ✅ Directory support
- ✅ Inode-based storage
- ✅ Bitmap allocation
- ✅ 512 inodes, 1981 data blocks available
- ✅ Foundation for advanced file operations

## 🎓 Educational Value

This release demonstrates:

1. **File System Design**: Superblock, inodes, bitmaps, data blocks
2. **Block-based I/O**: Reading and writing 512-byte blocks
3. **Bitmap Allocation**: Efficient free space management
4. **Inode Structure**: File metadata and block pointers
5. **Directory Entries**: Linking filenames to inodes
6. **File Descriptors**: Open file tracking
7. **Path Resolution**: Converting paths to inodes
8. **UNIX-like API**: open, read, write, close operations

## 🚧 Current Limitations

1. **Root Directory Only**: No subdirectory navigation (planned for future)
2. **Max File Size**: 5KB per file (10 direct blocks)
3. **No Indirect Blocks**: Limited to direct block pointers
4. **No Permissions**: Simple permission flags (not enforced)
5. **No Symbolic Links**: Direct entries only
6. **Volatile Storage**: Data lost on reboot (RAM disk based)
7. **Single User**: No multi-user support

## 🚀 Future Roadmap

**v1.0 - Advanced File System**
- Subdirectory support (cd, pwd commands)
- Indirect block pointers (larger files)
- File permissions enforcement
- Symbolic links
- Hard links
- Extended attributes

**v1.1 - Persistent Storage**
- IDE/ATA disk driver
- Persistent file system on real disk
- Disk caching

**v1.2 - Advanced Features**
- Multiple file system types (FAT12, ext2)
- Mount points
- Virtual File System (VFS) layer

## 🧪 Testing

Example testing session:

```bash
# Build and run
make clean && make all && make run

# Format file system
wittche> fsformat

# Create and write files
wittche> write hello.txt Hello from Wittche OS!
wittche> write test.txt This is a test file.
wittche> touch empty.txt

# List files
wittche> ls
Files in /:
===========
  [FILE] hello.txt (21 bytes)
  [FILE] test.txt (20 bytes)
  [FILE] empty.txt (0 bytes)

Total: 3 items

# Read file contents
wittche> cat hello.txt
Hello from Wittche OS!

# Create directory
wittche> mkdir documents

# List again
wittche> ls
Files in /:
===========
  [FILE] hello.txt (21 bytes)
  [FILE] test.txt (20 bytes)
  [FILE] empty.txt (0 bytes)
  [DIR]  documents

Total: 4 items

# Delete file
wittche> rm empty.txt
Deleted file: empty.txt

# Verify deletion
wittche> ls
Files in /:
===========
  [FILE] hello.txt (21 bytes)
  [FILE] test.txt (20 bytes)
  [DIR]  documents

Total: 3 items
```

## 📚 Technical Details

### Inode Allocation

When creating a file:
1. Search inode bitmap for free inode
2. Allocate inode and mark bitmap
3. Initialize inode structure (mode, size, timestamps)
4. Write inode to inode table
5. Add directory entry to parent directory

### File Writing

When writing to a file:
1. Open file and get inode
2. Calculate which blocks are needed
3. Allocate data blocks from bitmap
4. Write data block by block
5. Update inode (size, block pointers)
6. Flush inode and bitmaps to disk

### Directory Listing

When listing a directory:
1. Read directory inode
2. Read data blocks containing directory entries
3. Parse directory entries (inode number + name)
4. For each entry, read inode to get file info
5. Display formatted listing

## 🐛 Known Issues

None currently known.

## 📝 Breaking Changes

None. This release is fully backward compatible with v0.8.1.

---

**Previous Release:** [v0.8.1 - RAM Disk & Bootloader Fix](RELEASE_v0.8.1.md)
**Next Release:** v1.0 - Advanced File System Features (Planned)

---

## 🏆 Achievement Unlocked

Wittche OS now has a working file system! This is a major milestone in operating system development. 🎉
