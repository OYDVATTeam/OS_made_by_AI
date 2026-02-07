ASM      = nasm
LD       = ld
BUILD    = build
ISO_DIR  = iso
KERNEL   = $(ISO_DIR)/boot/kernel.bin
ISO      = simple-os.iso

.PHONY: all clean

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(KERNEL): boot/boot.asm linker.ld | $(BUILD)
	mkdir -p $(ISO_DIR)/boot
	$(ASM) -f elf32 boot/boot.asm -o $(BUILD)/kernel.o
	# REMOVED --oformat binary and added -T linker.ld
	$(LD) -m elf_i386 -T linker.ld $(BUILD)/kernel.o -o $(KERNEL)

$(ISO): $(KERNEL)
	grub-mkrescue -o $(ISO) $(ISO_DIR)
	@echo "Built $(ISO)"

clean:
	rm -rf $(BUILD) $(ISO) $(ISO_DIR)/boot/kernel.bin
