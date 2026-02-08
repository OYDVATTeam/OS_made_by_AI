# Makefile – kernel in src/, grub.cfg → iso/boot/grub/, linker.ld at root

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
KERNEL_BIN = $(BOOT_DIR)/kernel.bin           # placed in /boot/ inside ISO
ISO        = simple-os.iso

ASM_SRC    = boot/boot.asm
C_SRC      = src/kernel.c
OBJ        = $(BUILD)/boot.o $(BUILD)/kernel.o

.PHONY: all clean run iso_dirs

all: $(ISO)

iso_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(GRUB_DIR)               # ensures iso/boot/grub exists

$(BUILD)/boot.o: $(ASM_SRC) | iso_dirs
	$(ASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: $(C_SRC) | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJ) linker.ld | iso_dirs
	$(LD) $(LDFLAGS) -T linker.ld $(OBJ) -o $@

$(KERNEL_BIN): $(KERNEL_ELF) | iso_dirs
	cp $< $@                            # copy ELF to /boot/kernel.bin in ISO tree

$(ISO): $(KERNEL_BIN) grub.cfg | iso_dirs
	cp grub.cfg $(GRUB_DIR)/grub.cfg    # place config where GRUB expects it: /boot/grub/grub.cfg
	grub-mkrescue -o $@ $(ISO_DIR) 2>/dev/null || echo "grub-mkrescue failed — check xorriso/grub-pc-bin installed"
	@echo "ISO created: $@ (kernel at /boot/kernel.bin, config at /boot/grub/grub.cfg inside ISO)"

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)

# Optional: quick debug target (needs gdb-multiarch or similar)
debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -s -S -serial stdio &
	sleep 1
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_ELF)" -ex "break kernel_main" -ex "continue"
