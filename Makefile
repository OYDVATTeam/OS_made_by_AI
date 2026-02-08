# Makefile – Simple 32-bit OS kernel with mouse support

CC       = gcc
LD       = ld
ASM      = nasm

CFLAGS   = -m32 -ffreestanding -fno-pic -fno-stack-protector \
           -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables \
           -Wall -Wextra -O2 -nostdinc -nostdlib -nodefaultlibs \
           -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow \
           -fno-pie -march=i686 -Isrc

LDFLAGS  = -m elf_i386 -nostdlib

BUILD    = build
ISO_DIR  = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

KERNEL_ELF = $(BUILD)/kernel.elf
KERNEL_BIN = $(BOOT_DIR)/kernel.bin
ISO        = simple-os.iso

ASM_SRC = boot/boot.asm
C_SRCS  = src/kernel.c src/gui.c src/mouse.c

OBJ = $(BUILD)/boot.o \
      $(BUILD)/kernel.o \
      $(BUILD)/gui.o \
      $(BUILD)/mouse.o

.PHONY: all clean run iso_dirs

all: $(ISO)

iso_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(GRUB_DIR)

$(BUILD)/boot.o: $(ASM_SRC) | iso_dirs
	$(ASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: src/kernel.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/gui.o: src/gui.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/mouse.o: src/mouse.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJ) linker.ld | iso_dirs
	$(LD) $(LDFLAGS) -T linker.ld $(OBJ) -o $@

$(KERNEL_BIN): $(KERNEL_ELF) | iso_dirs
	cp $< $@

$(ISO): $(KERNEL_BIN) iso_dirs
	grub-mkrescue -o $@ $(ISO_DIR) || \
	echo "grub-mkrescue failed — install xorriso and grub-pc-bin"
	@echo "ISO created: $@"

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)
