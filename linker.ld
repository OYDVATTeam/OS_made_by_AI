/* linker.ld */
OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;           /* Kernel loaded at 1 MiB — standard for multiboot */

    .multiboot ALIGN(4) :
    {
        KEEP(*(.multiboot))   /* Multiboot header must be easy to find */
    }

    .text ALIGN(4K) :
    {
        *(.text)
        . = ALIGN(4K);
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata*)
        . = ALIGN(4K);
    }

    .data ALIGN(4K) :
    {
        *(.data)
        . = ALIGN(4K);
    }

    .bss ALIGN(4K) :
    {
        *(COMMON)
        *(.bss*)
        . = ALIGN(4K);
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
