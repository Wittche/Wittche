# Contributing to Wittche OS

First off, thank you for considering contributing to Wittche OS! 🎉

Wittche OS is an educational operating system project, and we welcome contributions from developers of all skill levels. Whether you're fixing bugs, adding features, improving documentation, or helping others, your contribution is valuable.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Coding Standards](#coding-standards)
- [Commit Guidelines](#commit-guidelines)
- [Pull Request Process](#pull-request-process)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [Documentation](#documentation)

## Code of Conduct

This project adheres to a Code of Conduct that all contributors are expected to follow. Please read [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) before contributing.

## How Can I Contribute?

### 🐛 Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates. When creating a bug report, include:

- **Clear title and description**
- **Steps to reproduce** the issue
- **Expected vs actual behavior**
- **System information** (OS, compiler versions)
- **Error messages or screenshots**

Use the bug report template when creating issues.

### 💡 Suggesting Enhancements

Enhancement suggestions are welcome! Please:

- **Check existing feature requests** first
- **Provide clear use cases** for the feature
- **Explain why this would be useful** for educational purposes
- **Consider the scope** - keep features focused and manageable

Use the feature request template when creating issues.

### 📝 Documentation

Documentation improvements are always appreciated:

- Fix typos or unclear explanations
- Add code examples
- Improve README or other docs
- Write tutorials or guides
- Add inline code comments

### 🔧 Code Contributions

#### Good First Issues

Look for issues labeled `good first issue` - these are great for newcomers!

Some ideas for contributions:

**Easy:**
- Add new shell commands
- Improve error messages
- Add color schemes
- Fix typos or formatting

**Medium:**
- Implement new drivers (serial port, etc.)
- Improve keyboard layout support
- Add string/utility functions
- Enhance screen driver features

**Advanced:**
- Memory management (paging, heap)
- Multitasking/scheduling
- File system support
- Network stack (far future!)

## Development Setup

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

### Development Workflow

1. **Fork** the repository
2. **Create a branch** for your feature: `git checkout -b feature/amazing-feature`
3. **Make your changes** and test thoroughly
4. **Commit** with clear messages: `git commit -m "Add amazing feature"`
5. **Push** to your fork: `git push origin feature/amazing-feature`
6. **Open a Pull Request**

## Coding Standards

### C Code Style

```c
// Use clear, descriptive names
void timer_init(void);  // Good
void ti(void);          // Bad

// Function documentation
/**
 * Initialize the Programmable Interval Timer
 * Sets up IRQ0 to fire at specified frequency
 */
void timer_init(void) {
    // Implementation
}

// Consistent indentation (4 spaces, no tabs)
if (condition) {
    do_something();
}

// Braces on same line for functions, control structures
void function(void) {
    if (x) {
        // code
    }
}
```

### Assembly Code Style

```asm
; Clear comments for every section
; Use consistent indentation
global _start

_start:
    mov eax, 0x01    ; System call number
    int 0x80         ; Call kernel
```

### General Guidelines

- ✅ Write **clear, self-documenting code**
- ✅ Add **comments for complex logic**
- ✅ Use **consistent naming conventions**
- ✅ Keep **functions focused and small**
- ✅ Avoid **magic numbers** - use named constants
- ✅ Check for **memory leaks** and **buffer overflows**
- ✅ Test on **real hardware** when possible (not just QEMU)

### File Organization

- **kernel/**: Kernel implementation files (.c)
- **include/**: Header files (.h)
- **boot/**: Bootloader code (.asm)
- **docs/**: Documentation (if any)

Each module should have:
- A `.c` implementation file
- A `.h` header file
- Clear separation of concerns

## Commit Guidelines

### Commit Message Format

```
<type>: <subject>

<body>

<footer>
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting)
- `refactor`: Code refactoring
- `test`: Adding tests
- `chore`: Build process, tools, etc.

**Examples:**

```
feat: Add serial port driver for COM1

Implement basic serial port output driver using I/O ports 0x3F8-0x3FF.
Useful for debugging and logging.

Closes #42
```

```
fix: Correct keyboard scancode for backslash key

The backslash key was mapped incorrectly in the US QWERTY layout.

Fixes #38
```

### Commit Best Practices

- ✅ Use **present tense** ("Add feature" not "Added feature")
- ✅ Use **imperative mood** ("Move cursor" not "Moves cursor")
- ✅ Limit **first line to 72 characters**
- ✅ Reference **issues and PRs** when relevant
- ✅ Keep commits **focused and atomic**

## Pull Request Process

### Before Submitting

1. ✅ **Test your changes** - ensure OS boots and runs
2. ✅ **Update documentation** if needed
3. ✅ **Follow coding standards**
4. ✅ **Write clear commit messages**
5. ✅ **Rebase on latest main** branch

### PR Description

Include:

- **What** changes were made
- **Why** the changes were necessary
- **How** to test the changes
- **Screenshots** (if UI changes)
- **Related issues** (Closes #XX)

### Review Process

- Maintainers will review your PR
- Address any feedback or requested changes
- Once approved, your PR will be merged!
- Your contribution will be credited

## Project Structure

```
Wittche/
├── boot/
│   └── boot.asm         # Bootloader
├── kernel/
│   ├── kernel.c         # Main kernel
│   ├── screen.c         # VGA driver
│   ├── keyboard.c       # Keyboard driver
│   ├── timer.c          # PIT timer
│   ├── idt.c            # Interrupt handling
│   ├── isr.c            # ISR handlers
│   └── shell.c          # Shell
├── include/
│   └── *.h              # Header files
├── Makefile             # Build system
├── README.md
├── CONTRIBUTING.md
└── LICENSE
```

## Testing

### Manual Testing

```bash
# Build and run
make run

# Test specific features
# - Type commands in shell
# - Check timer with 'uptime'
# - Test keyboard input
# - Verify screen scrolling
```

### Test Checklist

- [ ] OS boots successfully
- [ ] Shell prompt appears
- [ ] All commands work correctly
- [ ] Keyboard input is responsive
- [ ] Screen scrolling works
- [ ] Timer increments properly
- [ ] No crashes or exceptions

### Testing on Real Hardware

If possible, test on real hardware:

```bash
# Create bootable USB
dd if=build/wittche.img of=/dev/sdX bs=512
```

**Warning:** Be very careful with `dd` - double-check device name!

## Documentation

### Code Documentation

- Add **function-level comments** for all public APIs
- Explain **complex algorithms** with inline comments
- Document **I/O ports and hardware registers**
- Include **usage examples** where helpful

### README Updates

- Update feature list when adding capabilities
- Add new commands to command list
- Update roadmap when completing items
- Add credits for significant contributions

## Getting Help

- 💬 **Discussions**: Ask questions in GitHub Discussions
- 🐛 **Issues**: Report bugs or request features
- 📧 **Email**: Contact maintainers for sensitive issues

## Recognition

Contributors will be:

- 🏆 Listed in **AUTHORS** file
- 🙏 Credited in **release notes**
- ⭐ Appreciated in the **community**

## Resources

### Learning OS Development

- [OSDev Wiki](https://wiki.osdev.org/)
- [Intel Software Developer Manual](https://software.intel.com/en-us/articles/intel-sdm)
- [NASM Documentation](https://www.nasm.us/docs.php)
- [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf)

### Understanding Wittche OS

1. **Start with boot/boot.asm** - Understand the boot process
2. **Read kernel/kernel.c** - See initialization sequence
3. **Explore kernel/screen.c** - Learn VGA text mode
4. **Study kernel/idt.c** - Understand interrupts
5. **Look at kernel/shell.c** - See command processing

## Questions?

Feel free to ask! We're here to help you learn and contribute.

**Happy Coding!** 🚀
