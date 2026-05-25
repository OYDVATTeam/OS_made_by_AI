; boot.asm
BITS 32

; ─────────────────────────────────────────────────────────────────────────────
; Multiboot header (GRUB / multiboot compliant)
; ─────────────────────────────────────────────────────────────────────────────
SECTION .multiboot
align 4
multiboot_header:
    dd 0x1BADB002               ; magic
    dd 0x00000007               ; flags: Align(0) + MemInfo(1) + Video(2)
    dd -(0x1BADB002 + 0x00000007) ; checksum

    ; Graphics fields (Required because bit 2 is set)
    dd 0                        ; header_addr (unused for ELF)
    dd 0                        ; load_addr
    dd 0                        ; load_end_addr
    dd 0                        ; bss_end_addr
    dd 0                        ; entry_addr
    
    ; The specific request for 320x200x8
    dd 0                        ; mode_type (0 = graphics mode)
    dd 320                      ; width
    dd 200                      ; height
    dd 8                        ; depth (256 colors)

; ─────────────────────────────────────────────────────────────────────────────
; Entry point
; ─────────────────────────────────────────────────────────────────────────────
SECTION .text
global _start
extern kernel_main

_start:
    cli                         ; Disable interrupts

    ; Set up the stack
    mov esp, stack_top

    ; Push the Multiboot structure pointer (ebx) as an argument to kernel_main
    ; This allows you to find the actual Framebuffer address if it's not at 0xA0000
    push ebx 
    
    ; why the hell did i name the kernel entry this...
    call kernel_main

.hang:
    hlt
    jmp .hang

; ─────────────────────────────────────────────────────────────────────────────
; Stack and Notes
; ─────────────────────────────────────────────────────────────────────────────
SECTION .bss
align 16
stack_bottom:
    resb 16384                  ; 16 KiB stack
stack_top:

; to shut up linker
section .note.GNU-stack noalloc noexec nowrite progbits
