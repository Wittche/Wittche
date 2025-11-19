# Wittche Operating System

<div align="center">

![Version](https://img.shields.io/badge/version-0.5-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-x86-orange.svg)
![Language](https://img.shields.io/badge/language-C%20%7C%20Assembly-yellow.svg)

**An educational operating system built from scratch for learning OS development**

[Features](#features) • [Getting Started](#getting-started) • [Contributing](#contributing) • [Roadmap](#roadmap) • [Documentation](#documentation)

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

### 💻 Shell
- ✅ **Interactive Shell**: Full-featured command-line interface
- ✅ **Command Parser**: Argument parsing and tokenization
- ✅ **Command History**: Last 10 commands tracked
- ✅ **Built-in Commands**: help, clear/cls, about, ver, mem, echo, color, uptime, history, banner
- ✅ **Colorful Output**: Colored command output
- ✅ **Memory Inspector**: View memory layout and segment registers

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

- **[Contributing Guide](CONTRIBUTING.md)** - How to contribute to the project
- **[Code of Conduct](CODE_OF_CONDUCT.md)** - Community guidelines
- **[Roadmap](ROADMAP.md)** - Future plans and features
- **[Authors](AUTHORS)** - Contributors list

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

### Current Version: v0.4 ✅

See our [ROADMAP.md](ROADMAP.md) for detailed future plans!

### Upcoming Features

**v0.5** - Better Input & UX
- Cursor movement (arrow keys)
- Tab completion
- Printf-style formatting

**v0.6** - Memory Management
- Physical memory manager
- Paging (virtual memory)
- Heap allocator (kmalloc/kfree)

**v0.7** - Storage
- ATA/IDE disk driver
- FAT12 or custom file system
- File operations (ls, cat, etc.)

**v0.8** - Multitasking
- Process management
- Scheduler
- Context switching

**v1.0** - User Mode
- System calls
- User space programs
- Polish and stability

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
   - Initialize PIT timer
   - Initialize keyboard
   - Start shell

### Memory Layout
- **0x7C00**: Bootloader
- **0x10000**: Kernel code/data
- **0x90000**: Stack (grows down)
- **0xB8000**: VGA text buffer

### Interrupts
- **ISR 0-31**: CPU Exceptions
- **IRQ 32**: Timer (PIT)
- **IRQ 33**: Keyboard (PS/2)
- **IRQ 34-47**: Other hardware

## 🎨 Screenshots

```
===========================================
 Wittche Operating System v0.4
===========================================

Welcome to Wittche OS!
Type 'help' for available commands.

wittche> help

Available Commands:
==================
  help      - Display this help message
  clear     - Clear the screen
  about     - Show system information
  echo      - Echo a message
  color     - Test color output
  uptime    - Show system uptime
  history   - Show command history
  banner    - Display welcome banner

wittche> uptime

System Uptime:
  Time:        00:01:23
  Seconds:     83 s
  Ticks:       83000 (1000 Hz)

wittche> 
```

## 📊 Statistics

- **Language**: C (70%), Assembly (25%), Makefile (5%)
- **Lines of Code**: ~3,500
- **Modules**: 9 kernel modules
- **Commands**: 8 built-in shell commands
- **Interrupts**: 48 handlers (32 ISR + 16 IRQ)

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
