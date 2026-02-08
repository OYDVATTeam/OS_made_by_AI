# Makefile – no cross-compiler edition

CC       = gcc
LD       = ld
ASM      = nasm

CFLAGS   = -m32 -ffreestanding -fno-pic -fno-stack-protector \
           -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables \
           -Wall -Wextra -O2 -nostdinc -nostdlib -nodefaultlibs \
           -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow \
           -fno-pie -march=i686

LDFLAGS  = -m elf_i386 -nostdlib

BUILD    = build
ISO_DIR  = iso
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

KERNEL_ELF = $(BUILD)/kernel.elf
KERNEL_BIN = $(BOOT_DIR)/kernel.bin
ISO        = simple-os.iso

ASM_SRC    = boot/boot.asm
C_SRC      = kernel/kernel.c
OBJ        = $(BUILD)/boot.o $(BUILD)/kernel.o

.PHONY: all clean run iso_dirs

all: $(ISO)

iso_dirs:
	mkdir -p $(BUILD) $(BOOT_DIR) $(GRUB_DIR)

$(BUILD)/boot.o: $(ASM_SRC) | iso_dirs
	$(ASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: $(C_SRC) | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJ) linker.ld | iso_dirs
	$(LD) $(LDFLAGS) -T linker.ld $(OBJ) -o $@

$(KERNEL_BIN): $(KERNEL_ELF) | iso_dirs
	cp $< $@   # GRUB can multiboot ELF directly

$(ISO): $(KERNEL_BIN) grub.cfg | iso_dirs
	cp grub.cfg $(GRUB_DIR)/
	grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null || echo "grub-mkrescue warning"

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio
