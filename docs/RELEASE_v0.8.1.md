# Release Notes - v0.8.1 "RAM Disk & Bootloader Fix"

**Release Date:** November 20, 2024
**Type:** Feature Addition + Critical Bug Fix

---

## 🎯 Overview

Version 0.8.1 introduces RAM Disk functionality as the foundation for future file system implementation, along with a critical bootloader fix that was causing string literals to not load properly.

## ✨ What's New

### 💾 RAM Disk (Storage Foundation)

A fully functional 1MB virtual disk in memory that serves as the foundation for implementing a file system.

**Features:**
- **1MB Virtual Disk**: 2048 blocks × 512 bytes per block
- **Block I/O Operations**: Read and write individual 512-byte blocks
- **Format Support**: Clear/initialize the entire disk
- **Memory-based**: Lightning-fast access (memory speed)
- **Volatile**: Data is lost on reboot (as expected for RAM disk)

**New Shell Commands:**

```bash
# Display RAM disk information
wittche> rdinfo
RAM Disk Information
====================

Status:
  Initialized:     YES
  Base Address:    0x00200000

Configuration:
  Total Size:      1048576 bytes (1024 KB / 1 MB)
  Block Size:      512 bytes
  Block Count:     2048 blocks

Technical Details:
  Type:            Virtual disk in RAM
  Speed:           Memory speed (very fast)
  Volatile:        Yes (data lost on reboot)
  Purpose:         Foundation for file system

# Format (clear) RAM disk
wittche> rdformat
RAM Disk Format
===============

Formatting RAM disk (clearing all data)...
RAM Disk formatted successfully!
All blocks cleared to zero.

# Test RAM disk with comprehensive tests
wittche> ramdisk
RAM Disk Test
=============

Test 1: Write Pattern to Block 0
  Preparing test data...
  Writing block 0...
  SUCCESS: Block written

Test 2: Read and Verify Block 0
  Reading block 0...
  SUCCESS: Block read
  Verifying data...
  SUCCESS: Data verified (0 errors)

Test 3: Write ASCII Text to Block 1
  Writing: 'Hello from Wittche OS RAM Disk! This is a test message.'
  SUCCESS: Text written to block 1

Test 4: Read Text from Block 1
  Read back: 'Hello from Wittche OS RAM Disk! This is a test message.'
  SUCCESS: Text matches perfectly

Test 5: Multiple Block Operations
  Writing to 10 different blocks...
  Reading back and verifying...
  SUCCESS: All 10 blocks verified

RAM Disk Test Complete!
Total blocks available: 2048
Blocks tested: 12 (blocks 0, 1, and 10-19)
```

### 🐛 Critical Bug Fix: Bootloader Sector Loading

**Problem:** The bootloader was only loading 100 sectors (50KB) of the kernel. As the kernel grew larger, string literals stored in the `.rodata` section were not being loaded into memory, causing commands to display only numbers but no text.

**Symptoms:**
- Shell commands showed numbers but no text/letters
- `screen_write_dec()` worked (code was loaded)
- `screen_write()` with strings failed (strings were not loaded)
- Only affected functions in files that grew the kernel beyond 50KB

**Root Cause:**
String literals are stored in the `.rodata` (read-only data) section of the binary, which comes after the `.text` (code) section. When the kernel exceeded 50KB, the `.rodata` section was beyond the bootloader's load range.

**Solution:**
Implemented multi-read bootloader that properly handles BIOS limitations:

1. **BIOS Limitation**: `int 0x13` can only read ~63 sectors per call
2. **Multi-Read Implementation**:
   - Read 1: Cylinder 0, Head 0, Sectors 2-63 (62 sectors) → 0x10000
   - Read 2: Cylinder 0, Head 1, Sectors 1-63 (63 sectors) → 0x17C00
   - Total: 125 sectors (~64 KB) loaded contiguously

**Technical Details:**
```asm
; Read 1: Cyl 0, Head 0, Sectors 2-63 (62 sectors)
mov ah, 0x02        ; BIOS read function
mov al, 62          ; Number of sectors
mov ch, 0           ; Cylinder 0
mov cl, 2           ; Start from sector 2
mov dh, 0           ; Head 0
mov bx, 0x1000
mov es, bx
xor bx, bx
int 0x13

; Read 2: Cyl 0, Head 1, Sectors 1-63 (63 sectors)
; Segment calculation: 62*512 = 0x7C00
; Next address: 0x10000 + 0x7C00 = 0x17C00 = segment 0x17C0
mov ah, 0x02
mov al, 63
mov ch, 0
mov cl, 1
mov dh, 1           ; Head 1
mov bx, 0x17C0
mov es, bx
xor bx, bx
int 0x13
```

**For detailed analysis:** See [BOOTLOADER_STRING_LITERAL_FIX.md](BOOTLOADER_STRING_LITERAL_FIX.md)

## 📁 New Files

### Source Files
- `kernel/ramdisk.c` - RAM disk implementation
- `include/ramdisk.h` - RAM disk interface

### Documentation
- `docs/BOOTLOADER_STRING_LITERAL_FIX.md` - Detailed debugging case study
- `docs/RELEASE_v0.8.1.md` - This file

## 🔧 Modified Files

### Core Files
- `boot/boot.asm` - Multi-read bootloader implementation
- `kernel/shell.c` - Added `rdinfo`, `rdformat`, `ramdisk` commands
- `kernel/kernel.c` - Initialize RAM disk on boot
- `README.md` - Updated with v0.8.1 features and documentation

## 💻 API Changes

### New Functions (ramdisk.h)

```c
// Initialize RAM disk (called during kernel boot)
void ramdisk_init(void);

// Read a 512-byte block
int ramdisk_read_block(uint32_t block_num, void *buffer);

// Write a 512-byte block
int ramdisk_write_block(uint32_t block_num, const void *buffer);

// Format (clear) the entire disk
void ramdisk_format(void);

// Get RAM disk information
ramdisk_t *ramdisk_get_info(void);

// Check if initialized
int ramdisk_is_initialized(void);
```

### RAM Disk Structure

```c
typedef struct {
    uint8_t *data;           // Pointer to RAM disk data
    uint32_t size;           // Total size (1MB)
    uint32_t block_size;     // Block size (512 bytes)
    uint32_t block_count;    // Total blocks (2048)
    uint32_t initialized;    // Initialization flag
} ramdisk_t;
```

## 📊 Impact

### Before v0.8.1
- ❌ Bootloader loaded only 50KB
- ❌ String literals beyond 50KB not loaded
- ❌ RAM disk commands showed only numbers
- ❌ Kernel size limited to ~50KB

### After v0.8.1
- ✅ Bootloader loads 64KB (125 sectors)
- ✅ All string literals properly loaded
- ✅ All shell commands work correctly
- ✅ Kernel can grow to ~60KB before needing more sectors
- ✅ Foundation for file system ready

## 🚀 Future Roadmap

The RAM disk lays the groundwork for:

**v0.9 - User Mode & System Calls**
- Ring 3 user processes
- System call interface
- User/kernel separation

**v1.0 - File System**
- FAT12 or custom file system implementation
- File operations (create, read, write, delete)
- Directory support
- Persistence using RAM disk

## 🧪 Testing

All features have been tested in QEMU:

```bash
# Build and test
make clean && make all && make run

# Test RAM disk commands
wittche> rdinfo     # Display disk info
wittche> rdformat   # Format disk
wittche> ramdisk    # Run comprehensive tests
```

## 📚 Learning Resources

- **[Bootloader Debugging Case Study](BOOTLOADER_STRING_LITERAL_FIX.md)** - Deep dive into the bug fix
- **[OSDev Wiki - Disk Access](https://wiki.osdev.org/Disk_access_using_the_BIOS_(INT_13h))** - BIOS disk I/O
- **[RAM Disk Concept](https://en.wikipedia.org/wiki/RAM_drive)** - Understanding RAM disks

## 🙏 Acknowledgments

Special thanks to the user who identified the bootloader sector count as the potential issue - this critical insight led to solving a very difficult bug!

## 🐛 Known Issues

None currently known.

## 📝 Breaking Changes

None. This release is fully backward compatible.

---

**Previous Release:** [v0.8.0 - Advanced Process Features](.github/RELEASE_v0.8.0.md)
**Next Release:** v0.9 - User Mode & System Calls (Planned)
