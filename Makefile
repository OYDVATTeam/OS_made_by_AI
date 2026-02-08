# Makefile – kernel in src/, grub.cfg → iso/boot/grub/, linker.ld at root

CC       = gcc
LD       = ld
ASM      = nasm

# -Isrc allows headers like "vga.h" and "font.h" to be found easily
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

# --- Source Files ---
ASM_SRC    = boot/boot.asm
# Include kernel, gui, mouse, and your new font system
C_SRCS     = src/kernel.c src/gui.c src/mouse.c src/font.c

# Generate object file paths in the build directory
OBJ        = $(BUILD)/boot.o $(patsubst src/%.c, $(BUILD)/%.o, $(C_SRCS))

.PHONY: all clean run iso_dirs

all: $(ISO)

iso_dirs:
	@mkdir -p $(BUILD)
	@mkdir -p $(GRUB_DIR)

# Assemble bootloader
$(BUILD)/boot.o: $(ASM_SRC) | iso_dirs
	$(ASM) -f elf32 $< -o $@

# Compile C files (Pattern rule)
$(BUILD)/%.o: src/%.c | iso_dirs
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects using the linker script
$(KERNEL_ELF): $(OBJ) linker.ld | iso_dirs
	$(LD) $(LDFLAGS) -T linker.ld $(OBJ) -o $@

# Prepare for ISO creation
$(KERNEL_BIN): $(KERNEL_ELF) | iso_dirs
	cp $< $@

# Create the bootable ISO
$(ISO): $(KERNEL_BIN) iso_dirs
	grub-mkrescue -o $@ $(ISO_DIR) || echo "grub-mkrescue failed — check xorriso/grub-pc-bin installed"
	@echo "------------------------------------------"
	@echo "ISO created: $@"
	@echo "Objects linked: $(OBJ)"
	@echo "------------------------------------------"

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)

# Run in QEMU
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

# Debug with GDB
debug: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -s -S -serial stdio &
	sleep 1
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(KERNEL_ELF)" -ex "break kernel_main" -ex "continue"
