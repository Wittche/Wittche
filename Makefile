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

# Object files
OBJS = $(BUILD_DIR)/kernel_entry.o \
       $(BUILD_DIR)/kernel.o \
       $(BUILD_DIR)/screen.o \
       $(BUILD_DIR)/string.o \
       $(BUILD_DIR)/idt.o \
       $(BUILD_DIR)/isr.o \
       $(BUILD_DIR)/interrupt.o \
       $(BUILD_DIR)/keyboard.o \
       $(BUILD_DIR)/timer.o \
       $(BUILD_DIR)/shell.o

# Output files
BOOTLOADER = $(BUILD_DIR)/boot.bin
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

# Build kernel entry (special case for entry point)
$(BUILD_DIR)/kernel_entry.o: $(KERNEL_DIR)/kernel_entry.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

# Build C object files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $< -o $@

# Build ASM object files
$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.asm | $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(OBJS)
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
