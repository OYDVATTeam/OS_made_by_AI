ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS {
    . = 1M;
    .text : {
        *(.multiboot)
        *(.text)
    }
    .data : { *(.data) }
    .bss  : { *(.bss) }
}
