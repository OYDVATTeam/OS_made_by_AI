ASM     = nasm
BUILD   = build
ISO_DIR = iso
KERNEL  = $(ISO_DIR)/boot/kernel.bin
ISO     = simple-os.iso

.PHONY: all clean

all: $(ISO)

$(KERNEL): boot/boot.asm
	mkdir -p $(ISO_DIR)/boot
	$(ASM) -f elf32 boot/boot.asm -o $(BUILD)/kernel.o
	ld -m elf_i386 -Ttext 0x100000 --oformat binary \
	   $(BUILD)/kernel.o -o $(KERNEL)

$(ISO): $(KERNEL)
	grub-mkrescue -o $(ISO) $(ISO_DIR) >/dev/null 2>&1
	@echo "Built $(ISO)"

clean:
	rm -rf $(BUILD) $(ISO_DIR)/boot/kernel.bin $(ISO)
