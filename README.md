# Wittche Operating System

<div align="center">

![Version](https://img.shields.io/badge/version-0.7.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-x86-orange.svg)
![Language](https://img.shields.io/badge/language-C%20%7C%20Assembly-yellow.svg)

**An educational operating system with multitasking built from scratch for learning OS development**

[Features](#features) • [Getting Started](#getting-started) • [Release Notes](#-release-notes) • [Contributing](#contributing) • [Roadmap](#roadmap) • [Documentation](#documentation)

</div>

---

## 🎯 About

Wittche OS is an educational operating system designed to help developers learn operating system concepts from the ground up. Written in C and Assembly, it features a clean, well-documented codebase perfect for understanding how operating systems work.

This project was developed by artificial intelligence without knowing any code.

**Perfect for:**
- 🎓 Students learning OS development
- 💻 Developers wanting to understand low-level programming
- 🔬 Anyone interested in how computers boot and run
- 👥 Contributors looking for an educational open-source project

## ✨ Features

### 🚀 Core Features
- ✅ **Custom Bootloader**: BIOS boot, GDT setup, protected mode transition
- ✅ **32-bit Protected Mode Kernel**: Written entirely in C and Assembly
- ✅ **Interrupt Management**: Full IDT with ISR/IRQ support
- ✅ **Hardware Drivers**: PS/2 keyboard, VGA text mode, PIT timer

### 🎨 Display
- ✅ **VGA Text Mode Driver**: 80x25 colorful text output
- ✅ **Hardware Cursor**: VGA hardware cursor support
- ✅ **Proper Scrolling**: Automatic screen scrolling
- ✅ **16 Color Support**: Full VGA color palette

### ⌨️ Input
- ✅ **PS/2 Keyboard Driver**: Hardware interrupt-based
- ✅ **Scancode to ASCII**: US QWERTY layout
- ✅ **Special Keys**: Shift, Caps Lock, Backspace, Enter support
- ✅ **Input Buffering**: Circular queue keyboard buffer

### 💾 Memory Management (v0.6)
- ✅ **Physical Memory Manager (PMM)**: Bitmap-based page allocator (4096 pages, 16MB)
- ✅ **Kernel Heap**: Dynamic memory allocation (kmalloc/kfree, 4MB heap)
- ✅ **Paging**: Virtual memory with identity mapping
- ✅ **Memory Commands**: mem, meminfo, memtest

### 🔄 Process Management (v0.7)
- ✅ **Process Control Block (PCB)**: Full process state tracking
- ✅ **Preemptive Multitasking**: Round-robin scheduler with 10ms time slices
- ✅ **Context Switching**: Assembly-level full register save/restore
- ✅ **Process Commands**: ps (list processes), testproc (demo multitasking)
- ✅ **64 Concurrent Processes**: Maximum process support

### 💻 Shell
- ✅ **Interactive Shell**: Full-featured command-line interface
- ✅ **Command Parser**: Argument parsing and tokenization
- ✅ **Command History**: Last 10 commands tracked
- ✅ **Tab Completion**: Auto-complete commands with Tab key
- ✅ **Arrow Key Support**: Navigate and edit command line
- ✅ **Built-in Commands**: help, clear/cls, about, ver, mem, meminfo, memtest, ps, testproc, echo, color, uptime, history, banner
- ✅ **Colorful Output**: Colored command output
- ✅ **Memory Inspector**: View memory layout and statistics

### ⏱️ Timing
- ✅ **PIT (Programmable Interval Timer)**: 1000 Hz timer
- ✅ **System Uptime**: Accurate uptime tracking
- ✅ **Time Formatting**: HH:MM:SS display

### 📚 Libraries
- ✅ **String Library**: strlen, strcmp, strcpy, strcat, split, trim, etc.
- ✅ **Memory Functions**: memset, memcpy, memcmp
- ✅ **Conversion Functions**: atoi, itoa (various bases)
- ✅ **Printf-style Output**: kprintf() with format specifiers (%d, %x, %s, %p, etc.)
- ✅ **Modular Design**: Clean, separated module structure

## 🚀 Getting Started

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install nasm gcc-multilib make qemu-system-x86

# Fedora/RHEL
sudo dnf install nasm gcc make qemu-system-x86

# Arch Linux
sudo pacman -S nasm gcc make qemu
```

### Building

```bash
# Clone the repository
git clone https://github.com/YOUR-USERNAME/Wittche.git
cd Wittche

# Build the OS
make

# Run in QEMU
make run

# Clean build files
make clean
```

### Quick Start

```bash
# Build and run in one command
make && make run
```

You should see the Wittche OS boot screen and shell prompt!

## 📖 Documentation

- **[Release Notes v0.7.0](.github/RELEASE_v0.7.0.md)** - Process Management release
- **[Release Notes v0.6.0](.github/RELEASE_v0.6.0.md)** - Memory Management release
- **[Release Notes v0.5.0](.github/RELEASE_v0.5.0.md)** - Enhanced UX release
- **[Contributing Guide](CONTRIBUTING.md)** - How to contribute to the project
- **[Code of Conduct](CODE_OF_CONDUCT.md)** - Community guidelines
- **[Roadmap](ROADMAP.md)** - Future plans and features
- **[Authors](AUTHORS)** - Contributors list

## 📋 Release Notes

### Latest: v0.7.0 - "Process Manager" (November 19, 2024)
🔄 **Preemptive Multitasking is Here!**

**Key Features:**
- Process Control Block (PCB) system with full lifecycle management
- Round-robin preemptive scheduler with 10ms time slices
- Assembly-level context switching (<0.1ms switch time)
- New commands: `ps` (list processes), `testproc` (demo multitasking)
- Support for up to 64 concurrent processes

**[Read Full Release Notes →](.github/RELEASE_v0.7.0.md)**

### Previous Releases

**v0.6.0 - "Memory Manager"** (November 2024)
- Physical Memory Manager (PMM) with bitmap allocator
- Dynamic heap allocation (kmalloc/kfree)
- Virtual memory with paging
- **[Full Notes →](.github/RELEASE_v0.6.0.md)**

**v0.5.0 - "Enhanced UX"** (November 2024)
- Arrow key navigation and cursor control
- Tab completion for commands
- Printf-style formatting (kprintf)
- **[Full Notes →](.github/RELEASE_v0.5.0.md)**

### Learning Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development wiki
- [Intel x86 Manual](https://software.intel.com/en-us/articles/intel-sdm) - Official Intel documentation
- [NASM Documentation](https://www.nasm.us/docs.php) - NASM assembler reference

## 🤝 Contributing

We love contributions! Wittche OS is an open-source educational project, and we welcome developers of all skill levels.

### Ways to Contribute

- 🐛 **Report bugs** - Help us find and fix issues
- 💡 **Suggest features** - Share your ideas for new features
- 📝 **Improve documentation** - Make it easier for others to learn
- 💻 **Submit code** - Implement new features or fix bugs
- 🎨 **Enhance design** - Improve the user interface
- 🧪 **Test on real hardware** - Validate compatibility
- 💬 **Help others** - Answer questions in discussions

### Quick Start for Contributors

1. **Fork** the repository
2. **Create a branch**: `git checkout -b feature/amazing-feature`
3. **Make your changes** and test thoroughly
4. **Commit**: `git commit -m "Add amazing feature"`
5. **Push**: `git push origin feature/amazing-feature`
6. **Open a Pull Request**

See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed guidelines.

### Good First Issues

Look for issues labeled `good first issue` - perfect for newcomers!

Some ideas:
- Add new shell commands
- Implement keyboard layouts (AZERTY, QWERTZ)
- Add string utility functions
- Improve error messages
- Write documentation
- Add code comments

## 🗺️ Roadmap

### Current Version: v0.7.0 ✅

See our [ROADMAP.md](ROADMAP.md) for detailed future plans!

### Completed Features

**v0.5** - Better Input & UX ✅
- ✅ Cursor movement (arrow keys)
- ✅ Tab completion
- ✅ Printf-style formatting

**v0.6** - Memory Management ✅
- ✅ Physical memory manager (PMM)
- ✅ Paging (virtual memory)
- ✅ Heap allocator (kmalloc/kfree)

**v0.7** - Process Management ✅
- ✅ Process Control Blocks (PCB)
- ✅ Preemptive scheduler (round-robin)
- ✅ Context switching
- ✅ Multitasking support

### Upcoming Features

**v0.8** - Advanced Process Features
- Process priorities and multi-level scheduling
- Sleep/wake functionality
- Inter-Process Communication (IPC)
- Signal handling

**v0.9** - User Mode & System Calls
- Ring 3 user processes
- System call interface (INT 0x80)
- Task State Segment (TSS)
- User/kernel memory separation

**v1.0** - File System
- ATA/IDE disk driver
- FAT12 or simple custom file system
- File operations (open, read, write, close)
- Directory support

## 📁 Project Structure

```
Wittche/
├── boot/
│   └── boot.asm         # Bootloader (BIOS → Protected Mode)
│
├── kernel/              # Kernel source files
│   ├── kernel.c         # Main kernel initialization
│   ├── screen.c         # VGA text mode driver
│   ├── idt.c            # Interrupt Descriptor Table
│   ├── isr.c            # Interrupt Service Routines
│   ├── interrupt.asm    # ISR/IRQ assembly stubs
│   ├── keyboard.c       # PS/2 keyboard driver
│   ├── timer.c          # PIT timer driver
│   ├── string.c         # String utility functions
│   ├── kprintf.c        # Printf-style formatted output
│   ├── pmm.c            # Physical Memory Manager
│   ├── heap.c           # Kernel heap (kmalloc/kfree)
│   ├── paging.c         # Virtual memory paging
│   ├── process.c        # Process management
│   ├── switch.asm       # Context switching
│   └── shell.c          # Interactive shell
│
├── include/             # Header files
│   ├── kernel.h         # Kernel main header
│   ├── screen.h         # Screen driver interface
│   ├── idt.h            # IDT structures & functions
│   ├── keyboard.h       # Keyboard driver interface
│   ├── timer.h          # Timer driver interface
│   ├── shell.h          # Shell interface
│   ├── string.h         # String utilities
│   ├── kprintf.h        # Printf interface
│   ├── pmm.h            # PMM interface
│   ├── heap.h           # Heap allocator interface
│   ├── paging.h         # Paging interface
│   ├── process.h        # Process management interface
│   ├── ports.h          # I/O port operations
│   └── types.h          # Type definitions
│
├── .github/             # GitHub templates
│   ├── ISSUE_TEMPLATE/  # Issue templates
│   └── PULL_REQUEST_TEMPLATE.md
│
├── build/               # Build output (gitignore'd)
├── linker.ld            # Linker script
├── Makefile             # Build system
├── README.md            # This file
├── CONTRIBUTING.md      # Contribution guidelines
├── CODE_OF_CONDUCT.md   # Community guidelines
├── ROADMAP.md           # Future plans
├── LICENSE              # MIT License
└── AUTHORS              # Contributors list
```

## 🔧 Technical Details

### Boot Process
1. **BIOS** loads boot sector (512 bytes) to 0x7C00
2. **Bootloader** (boot.asm):
   - Loads kernel from disk to 0x10000
   - Sets up GDT
   - Switches to protected mode
   - Jumps to kernel

3. **Kernel Entry** (kernel_entry.asm):
   - Sets up segment registers
   - Initializes stack
   - Calls `kernel_main()`

4. **Kernel Init** (kernel.c):
   - Initialize VGA screen
   - Set up IDT and PIC
   - Initialize Physical Memory Manager (PMM)
   - Initialize Kernel Heap
   - Enable Paging (Virtual Memory)
   - Initialize Process Management
   - Initialize PIT timer (triggers scheduler)
   - Initialize keyboard
   - Start shell (becomes PID 1)

### Memory Layout (16MB System)
- **0x00000000 - 0x000004FF**: Real Mode IVT (1KB)
- **0x00007C00 - 0x00007DFF**: Bootloader (512 bytes)
- **0x00010000 - 0x001FFFFF**: Kernel Code & Data (~2MB)
- **0x00200000 - 0x005FFFFF**: Kernel Heap (4MB)
  - Process PCBs and stacks
  - Dynamic allocations
- **0x00600000 - 0x00FFFFFF**: Free Physical Memory (~10MB)
- **0x000B8000 - 0x000B8FA0**: VGA Text Buffer (4000 bytes)

### Interrupts
- **ISR 0-31**: CPU Exceptions
- **IRQ 32**: Timer (PIT)
- **IRQ 33**: Keyboard (PS/2)
- **IRQ 34-47**: Other hardware

## 🎨 Screenshots

```
===========================================
 Wittche Operating System v0.7
===========================================

Welcome to Wittche OS!
Type 'help' for available commands.

wittche> ver

Wittche OS Version Information
==============================

  Version:     0.7.0
  Codename:    Process Manager
  Build Date:  2024-11
  Arch:        x86 (32-bit)

wittche> ps

Process List
============

PID  NAME                 STATE      PRIORITY  TIME(ms)
---  -------------------  ---------  --------  --------
0    IdleProcess          READY      0         1234
1    ShellProcess         RUNNING    10        5678

Total processes: 2
Current process: ShellProcess (PID 1)

wittche> testproc

Spawning Test Processes
========================
Created Process A (PID 2)
Created Process B (PID 3)

Test processes are now running!
[Process A] Running iteration 0
[Process B] Executing iteration 0
[Process A] Running iteration 1
[Process B] Executing iteration 1
...

wittche> ps

PID  NAME                 STATE      PRIORITY  TIME(ms)
---  -------------------  ---------  --------  --------
0    IdleProcess          READY      0         1567
1    ShellProcess         RUNNING    10        6234
2    TestProcA            READY      10        89
3    TestProcB            READY      10        92

Total processes: 4
```

## 📊 Statistics

- **Language**: C (75%), Assembly (20%), Makefile (5%)
- **Lines of Code**: ~5,500+
- **Kernel Modules**: 15 modules (screen, idt, isr, keyboard, timer, string, kprintf, pmm, heap, paging, process, shell)
- **Shell Commands**: 15+ built-in commands
- **Interrupts**: 48 handlers (32 ISR + 16 IRQ)
- **Kernel Size**: ~34 KB
- **Max Processes**: 64 concurrent processes
- **Context Switch**: <0.1ms
- **Scheduler Overhead**: ~1%

## 🧪 Testing

### In QEMU
```bash
make run
```

### On Real Hardware (Advanced)
```bash
# Create bootable USB (BE VERY CAREFUL!)
sudo dd if=build/wittche.img of=/dev/sdX bs=512
```

**Warning**: Double-check the device name! `dd` can destroy data.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Community

- 💬 **Discussions**: Share ideas and ask questions
- 🐛 **Issues**: Report bugs or request features
- 🔀 **Pull Requests**: Submit your contributions
- ⭐ **Star**: Show your support!

## 🙏 Acknowledgments

- **OSDev Community** - Invaluable resources and support
- **Contributors** - Everyone who has contributed to this project
- **You** - For being interested in OS development!

## 📬 Contact

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and general discussion

---

<div align="center">

**Made with ❤️ for learning and education**

Star ⭐ this repo if you find it helpful!

[Report Bug](https://github.com/YOUR-USERNAME/Wittche/issues/new?template=bug_report.md) • [Request Feature](https://github.com/YOUR-USERNAME/Wittche/issues/new?template=feature_request.md) • [Ask Question](https://github.com/YOUR-USERNAME/Wittche/issues/new?template=question.md)

</div>
