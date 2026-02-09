# Makefile – kernel in src/, libc in src/libc/, linker.ld at root

CC       = gcc
LD       = ld
ASM      = nasm

# Added -Isrc/libc so you can #include "string.h" easily
CFLAGS   = -m32 -ffreestanding -fno-pic -fno-stack-protector \
           -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables \
           -Wall -Wextra -O2 -nostdinc -nostdlib -nodefaultlibs \
           -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow \
           -fno-pie -march=i686 -Isrc -Isrc/libc

LDFLAGS  = -m elf_i386 -nostdlib

BUILD    = build
ISO_DIR  = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

KERNEL_ELF = $(BUILD)/kernel.elf
KERNEL_BIN = $(BOOT_DIR)/kernel.bin
ISO        = simple-os.iso

ASM_SRC    = boot/boot.asm

# --- SOURCE DISCOVERY ---
# Main kernel sources
C_SRCS     = src/kernel.c src/gui.c src/mouse.c src/font.c src/ata.c src/fat.c

# Libc sources
LIBC_SRCS  = src/libc/string.c src/libc/stdio.c src/libc/stdlib.c src/libc/ctype.c

# Combine all objects
OBJ        = $(BUILD)/boot.o \
             $(patsubst src/%.c, $(BUILD)/%.o, $(C_SRCS)) \
             $(patsubst src/libc/%.c, $(BUILD)/libc/%.o, $(LIBC_SRCS))

.PHONY: all clean run iso_dirs

all: $(ISO)

# Ensure folders exist (Added libc build dir)
iso_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/libc
	@mkdir -p $(GRUB_DIR)

# Assemble bootloader
$(BUILD)/boot.o: $(ASM_SRC) | iso_dirs
	$(ASM) -f elf32 $< -o $@

# Compile main C files
$(BUILD)/%.o: src/%.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Libc files (Separate rule for nested directory)
$(BUILD)/libc/%.o: src/libc/%.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects
$(KERNEL_ELF): $(OBJ) linker.ld | iso_dirs
	$(LD) $(LDFLAGS) -T linker.ld $(OBJ) -o $@

# Copy kernel to ISO folder
$(KERNEL_BIN): $(KERNEL_ELF) | iso_dirs
	cp $< $@

# Create ISO
$(ISO): $(KERNEL_BIN)
	@if [ ! -f $(GRUB_DIR)/grub.cfg ]; then \
		echo "Warning: grub.cfg missing in $(GRUB_DIR)! Generating basic one..."; \
		echo 'set timeout=0' > $(GRUB_DIR)/grub.cfg; \
		echo 'set default=0' >> $(GRUB_DIR)/grub.cfg; \
		echo 'menuentry "Gemini OS" { multiboot /boot/kernel.bin; boot }' >> $(GRUB_DIR)/grub.cfg; \
	fi
	grub-mkrescue -o $@ $(ISO_DIR)

clean:
	rm -rf $(BUILD)
	rm -f $(ISO)
	rm -f $(KERNEL_BIN)
	@echo "Cleaned build artifacts."

# Run with Hard Drive support (Added -drive for your FAT testing)
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -drive file=disk.img,format=raw,index=0,media=disk
