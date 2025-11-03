# Makefile for Wittche OS

# Tools
ASM = nasm
CC = gcc
LD = ld

# Flags
ASM_FLAGS = -f elf32
CC_FLAGS = -m32 -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c
LD_FLAGS = -m elf_i386 -T linker.ld

# Directories
BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel

# Files
BOOTLOADER = $(BUILD_DIR)/boot.bin
KERNEL_ENTRY = $(BUILD_DIR)/kernel_entry.o
KERNEL_OBJ = $(BUILD_DIR)/kernel.o
IDT_OBJ = $(BUILD_DIR)/idt.o
ISR_OBJ = $(BUILD_DIR)/isr.o
INTERRUPT_OBJ = $(BUILD_DIR)/interrupt.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
OS_IMAGE = $(BUILD_DIR)/wittche.img

# Default target
all: $(OS_IMAGE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build bootloader
$(BOOTLOADER): $(BOOT_DIR)/boot.asm | $(BUILD_DIR)
	$(ASM) -f bin $< -o $@

# Build kernel entry
$(KERNEL_ENTRY): $(KERNEL_DIR)/kernel_entry.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

# Build kernel C code
$(KERNEL_OBJ): $(KERNEL_DIR)/kernel.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

# Build IDT
$(IDT_OBJ): $(KERNEL_DIR)/idt.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

# Build ISR
$(ISR_OBJ): $(KERNEL_DIR)/isr.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

# Build interrupt handlers
$(INTERRUPT_OBJ): $(KERNEL_DIR)/interrupt.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

# Build keyboard driver
$(KEYBOARD_OBJ): $(KERNEL_DIR)/keyboard.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_ENTRY) $(KERNEL_OBJ) $(IDT_OBJ) $(ISR_OBJ) $(INTERRUPT_OBJ) $(KEYBOARD_OBJ)
	$(LD) $(LD_FLAGS) $^ -o $@

# Create OS image
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL_BIN)
	cat $(BOOTLOADER) $(KERNEL_BIN) > $@
	# Pad to 1.44MB (floppy disk size)
	truncate -s 1474560 $@

# Run with QEMU
run: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE)

# Run with QEMU in debug mode
debug: $(OS_IMAGE)
	qemu-system-i386 -drive format=raw,file=$(OS_IMAGE) -s -S

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all run debug clean
