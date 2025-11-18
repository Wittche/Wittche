# Wittche OS Roadmap

This document outlines the development roadmap for Wittche OS. Our goal is to create an educational operating system that helps people learn OS development concepts.

## Current Version: v0.4

**Release Date**: November 2024

### Completed Features ✅

- [x] Custom bootloader (BIOS → Protected Mode)
- [x] Protected mode kernel (32-bit)
- [x] GDT (Global Descriptor Table)
- [x] IDT (Interrupt Descriptor Table)
- [x] ISR/IRQ handlers (32 exceptions + 16 IRQs)
- [x] PIC (Programmable Interrupt Controller) configuration
- [x] PIT (Programmable Interval Timer) @ 1000 Hz
- [x] VGA text mode driver (80x25, 16 colors)
- [x] Hardware cursor support
- [x] Proper screen scrolling
- [x] PS/2 keyboard driver
- [x] Scancode to ASCII conversion (US QWERTY)
- [x] Keyboard input buffering (circular queue)
- [x] Interactive shell
- [x] Command parser (tokenization, arguments)
- [x] Command history (10 entries)
- [x] String library (strlen, strcmp, strcpy, etc.)
- [x] Memory utilities (memset, memcpy, memcmp)
- [x] Conversion functions (atoi, itoa)
- [x] System uptime tracking
- [x] Shell commands: help, clear, about, echo, color, uptime, history, banner

---

## Version 0.5 (Next Release)

**Target**: Q1 2025
**Focus**: Enhanced User Experience & Input

### Planned Features

#### Better Line Editing
- [ ] **Cursor movement** (left/right arrows)
- [ ] **Home/End** keys
- [ ] **Delete** key support
- [ ] **Insert** mode toggle
- [ ] Visual cursor in input line

#### Enhanced Keyboard
- [ ] **Up/Down arrows** for history navigation
- [ ] **Tab completion** for commands
- [ ] **Ctrl+C** interrupt support
- [ ] **Ctrl+L** clear screen shortcut
- [ ] **Alt** key support

#### Printf-style Formatting
- [ ] `kprintf()` function
- [ ] Format specifiers: %d, %x, %s, %c
- [ ] Width and padding support
- [ ] Color formatting codes

#### Additional Shell Commands
- [ ] `time` - Show current uptime in detail
- [ ] `cls` - Alias for clear
- [ ] `ver` - Show version info
- [ ] `mem` - Memory information (preparation for v0.6)

### Issues to Address
- Improve backspace handling at line start
- Better error messages for invalid commands
- Command line length limit indicator

---

## Version 0.6

**Target**: Q2 2025
**Focus**: Memory Management

### Memory Management
- [ ] **Physical memory manager**
  - Memory map from bootloader
  - Bitmap/stack-based allocator
  - Free page tracking

- [ ] **Virtual memory (Paging)**
  - Page tables and directories
  - Identity mapping for kernel
  - Enable paging (CR0.PG)

- [ ] **Heap allocator**
  - `kmalloc()` - Allocate memory
  - `kfree()` - Free memory
  - `krealloc()` - Reallocate memory
  - First-fit or best-fit algorithm

- [ ] **Memory commands**
  - `free` - Show memory usage
  - `memtest` - Test memory allocator

### Benefits
- Foundation for multitasking
- Dynamic memory allocation
- Better resource management

---

## Version 0.7

**Target**: Q3 2025
**Focus**: Storage & File System

### Disk Driver
- [ ] **ATA/IDE driver**
  - PIO mode 28-bit LBA
  - Sector read/write
  - Device detection

### File System
- [ ] **FAT12 implementation** or **Custom FS**
  - Boot sector parsing
  - File allocation table
  - Root directory reading
  - File read operations

- [ ] **VFS layer**
  - Virtual file system interface
  - Mount points
  - File descriptors

### File Commands
- [ ] `ls` - List directory
- [ ] `cat` - Display file contents
- [ ] `mkdir` - Create directory
- [ ] `rm` - Remove file
- [ ] `cd` - Change directory
- [ ] `pwd` - Print working directory

---

## Version 0.8

**Target**: Q4 2025
**Focus**: Multitasking

### Process Management
- [ ] **Task structures**
  - Process control block (PCB)
  - Process states (ready, running, blocked)
  - Context switching

- [ ] **Scheduler**
  - Round-robin scheduling
  - Priority levels (optional)
  - Timer-based preemption

- [ ] **Basic processes**
  - Idle process
  - Test processes
  - Process creation/termination

### Process Commands
- [ ] `ps` - List processes
- [ ] `kill` - Terminate process
- [ ] `top` - Process monitor

---

## Version 0.9

**Target**: Q1 2026
**Focus**: User Mode & System Calls

### User Mode
- [ ] **TSS (Task State Segment)**
- [ ] **User mode transition**
- [ ] **Privilege levels** (Ring 0 vs Ring 3)

### System Calls
- [ ] **Syscall interface**
  - `int 0x80` handler
  - Parameter passing
  - Return values

- [ ] **Core syscalls**
  - `sys_write()` - Write to screen
  - `sys_read()` - Read input
  - `sys_fork()` - Create process
  - `sys_exit()` - Terminate process
  - `sys_open()`, `sys_close()`, etc.

### User Programs
- [ ] Simple user-space programs
- [ ] ELF binary loader (optional)

---

## Version 1.0

**Target**: Q2 2026
**Focus**: Polish & Stability

### Goals
- [ ] Stable, well-tested kernel
- [ ] Comprehensive documentation
- [ ] Tutorial series
- [ ] Real hardware testing

### Nice-to-Have Features
- [ ] Serial port driver (debugging)
- [ ] VESA graphics mode (optional)
- [ ] Sound (PC speaker beeps)
- [ ] Simple games/demos
- [ ] Network stack (very ambitious!)

---

## Long-Term Vision (Post 1.0)

### Advanced Features
- [ ] **SMP (Symmetric Multiprocessing)**
  - Multi-core support
  - APIC instead of PIC

- [ ] **Advanced memory**
  - Demand paging
  - Swap support
  - Memory-mapped files

- [ ] **Networking**
  - Network drivers (e1000, rtl8139)
  - TCP/IP stack
  - Socket API

- [ ] **Graphics**
  - VESA framebuffer
  - Basic GUI
  - Window manager

- [ ] **Ports**
  - x86-64 (64-bit)
  - ARM support
  - UEFI boot

---

## How to Contribute

We welcome contributions! Here's how you can help:

### Pick a Feature
1. Check the roadmap
2. Look for unchecked items
3. Open an issue to discuss
4. Submit a PR when ready

### Difficulty Levels

**🟢 Easy** (Good First Issues)
- Shell commands
- String utilities
- Documentation
- Bug fixes

**🟡 Medium**
- Keyboard layouts
- Printf implementation
- Screen enhancements
- Serial driver

**🔴 Hard**
- Memory management
- File system
- Multitasking
- User mode

**🟣 Expert**
- SMP support
- Networking
- 64-bit port
- UEFI boot

---

## Priorities

Our development priorities (in order):

1. **Education** - Code should be readable and well-documented
2. **Stability** - Features should work reliably
3. **Simplicity** - Avoid over-engineering
4. **Completeness** - Finish features before starting new ones
5. **Performance** - Optimize only when necessary

---

## Release Schedule

- **Minor versions** (0.x): Every 2-3 months
- **Major version** (1.0): When core features are complete
- **Patch releases**: As needed for bugs

---

## Community Feedback

This roadmap is not set in stone! We welcome:
- Feature suggestions
- Priority changes
- Timeline adjustments
- New ideas

Open an issue or discussion to share your thoughts!

---

**Last Updated**: November 2024
**Version**: 0.4
**Status**: Active Development 🚀
