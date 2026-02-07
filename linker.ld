ENTRY(_start)
SECTIONS
{
    /* GRUB loads kernels at 1MB by convention */
    . = 1M;

    .text :
    {
        /* Force the multiboot header to the front */
        KEEP(*(.multiboot))
        *(.text)
    }

    .data : { *(.data) }
    .bss  : { *(.bss) }
}
