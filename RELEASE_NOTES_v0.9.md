# Wittche OS v0.9.0 Release Notes

**Release Date:** November 19, 2025
**Codename:** Virtual Liberty

---

## Overview

Wittche OS v0.9.0 represents a major milestone in operating system development, introducing **true user mode execution** with complete memory isolation through virtual memory management. This release implements the foundation for running untrusted user programs safely in Ring 3 with full privilege separation from the kernel.

---

## 🎯 Major Features

### 1. **Per-Process Virtual Memory**
- Each process now has its own isolated page directory
- Kernel processes share the kernel page directory (identity-mapped first 16MB)
- User mode processes get dedicated page directories with kernel mappings
- Context switches automatically switch page directories for complete isolation
- Proper cleanup of page directories on process termination

**Technical Details:**
- `page_directory_t *` added to process structure
- `paging_create_user_directory()` creates isolated address spaces
- `paging_switch_directory()` called on every process switch
- `paging_free_directory()` cleans up all user pages and page tables

### 2. **User Code Mapping (Ring 3 Execution)**
- User code mapped to virtual address `0x40000000` (1GB mark)
- User stack mapped to virtual address `0x80000000` (2GB mark)
- Entry point code copied to physical pages
- Physical pages mapped with `PAGE_USER_MODE` flags (Ring 3 accessible)
- Enables true privilege separation between kernel and user processes

**Memory Layout:**
```
0x00000000 - 0x01000000: Kernel (16MB, identity-mapped)
0x40000000 - 0x40040000: User Code (256KB max)
0x7FF00000 - 0x80000000: User Stack (1MB max, grows down)
```

**Implementation:**
- `USER_CODE_BASE = 0x40000000` for user code
- `USER_STACK_BASE = 0x80000000` for user stack
- `paging_map_user_code()` maps physical pages to virtual addresses
- Process EIP set to `USER_CODE_BASE` for user processes

### 3. **Enhanced Page Fault Handler**
- Comprehensive error code parsing and reporting
- Displays fault type (present vs protection violation)
- Shows access mode (read/write/execute)
- Identifies privilege level (kernel vs user mode)
- Reports page directory and table indices
- Shows current process information (PID, name, mode)
- Color-coded output for better readability

**Error Code Decoding:**
- Bit 0 (P): Page not present vs protection violation
- Bit 1 (W/R): Read vs write access
- Bit 2 (U/S): Kernel mode vs user mode
- Bit 3 (RSVD): Reserved bit violation
- Bit 4 (I/D): Instruction fetch

### 4. **Demand Paging**
- Automatic page allocation on first access (lazy allocation)
- Reduces initial memory footprint
- Supports dynamic stack growth (up to 1MB)
- Heap region allocation on demand
- Only allocates pages when actually needed

**Supported Regions:**
- Stack: `0x7FF00000 - 0x80000000` (1MB max, grows down from 2GB)
- Heap: `0x40040000 - 0x80000000` (after user code, before stack)

**How It Works:**
1. Page fault occurs for unmapped user page
2. Handler checks if address is in valid user region
3. Allocates physical page via `pmm_alloc_page()`
4. Maps page with `PAGE_USER_MODE` flags
5. Returns to retry faulting instruction

---

## 🔧 Technical Improvements

### Memory Management
- Per-process page directories for complete isolation
- User virtual address space separate from kernel
- Copy-on-write preparation (infrastructure ready)
- TLB flushing on page directory switch

### Process Management
- Page directory field added to process structure
- Automatic page directory allocation for user processes
- Page directory cleanup on process termination
- Context switch includes page directory switch

### Paging System
- `paging_create_user_directory()`: Create isolated address space
- `paging_free_directory()`: Clean up all user pages
- `paging_map_user_code()`: Map code/data to virtual memory
- Demand paging for stack and heap regions

### Debugging
- Detailed page fault diagnostics
- Process information in fault handler
- Memory region identification
- Color-coded error messages

---

## 📊 System Statistics

- **Kernel Size:** 51 KB
- **Shell Commands:** 21
- **System Calls:** 8
- **Memory Layout:** Identity-mapped kernel + isolated user space
- **Max User Stack:** 1 MB
- **Max User Code:** 256 KB

---

## 🧪 Testing

### Updated Commands

#### `usermodetest`
Now creates a **real Ring 3 user mode process** with:
- Isolated virtual address space
- User code at `0x40000000`
- User stack at `0x80000000`
- System calls via INT 0x80
- Full privilege separation

**Example Output:**
```
Ring 3 User Mode Test
======================
Creating user mode process with virtual memory...

[USERMODE] Mapped user code: virt=0x40000000, phys=0x00123000
[USERMODE] Mapped user stack: virt=0x7FFFF000, phys=0x00124000
Created Ring 3 Process (PID 2)

User mode features:
  - Isolated virtual address space
  - Ring 3 execution (CPL=3)
  - System calls via INT 0x80
  - User code at 0x40000000
  - User stack at 0x80000000

Watch the process run in Ring 3!

[Ring 3] Hi!
PID=2
Sleep..
OK!
```

---

## 🔐 Security Enhancements

1. **Memory Isolation**
   - User processes cannot access kernel memory
   - User processes cannot access other process memory
   - Page-level protection enforced by CPU

2. **Privilege Separation**
   - User code runs in Ring 3 (unprivileged)
   - Kernel code runs in Ring 0 (privileged)
   - System calls required for kernel services

3. **Error Handling**
   - Protection violations caught by page fault handler
   - Invalid memory access detected and reported
   - Process crashes don't affect kernel

---

## 🚀 What's Next (v1.0.0)

The foundation is now complete for a full-featured OS. Planned for v1.0:

### File System
- VFS (Virtual File System) layer
- FAT-like file system implementation
- File operations (open, read, write, close)
- Directory operations

### ELF Binary Loader
- Load ELF executables from disk
- Separate .text, .data, .bss sections
- Dynamic linking preparation
- User program loading

### Advanced IPC
- Pipes for inter-process communication
- Shared memory regions
- Semaphores for synchronization
- Message queues enhancement

### Device Drivers
- Block device abstraction
- Character device support
- Device manager

---

## 📝 Commits in This Release

1. **cc769cc** - Add per-process virtual memory support
2. **00d297a** - Implement user code mapping for Ring 3 execution
3. **3eaf0bc** - Enhance page fault handler with detailed diagnostics
4. **6c4b58f** - Implement demand paging for dynamic memory allocation

---

## 🎓 Educational Value

This release demonstrates:
- Virtual memory management with paging
- Privilege levels and CPU protection rings
- Memory isolation techniques
- Demand paging and lazy allocation
- Page fault handling and recovery
- System call interface for user mode

Perfect for students learning:
- Operating system internals
- Memory management
- Process isolation
- x86 architecture

---

## 🙏 Acknowledgments

This release builds upon the solid foundation of:
- v0.7.0: Process management and scheduling
- v0.8.0: IPC and system calls
- Community feedback and suggestions

---

## 📄 License

Wittche OS is released under the MIT License.

---

**Happy Hacking!** 🚀

For questions, issues, or contributions, please visit:
https://github.com/Wittche/Wittche
