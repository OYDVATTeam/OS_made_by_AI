; boot.asm
BITS 32

; ─────────────────────────────────────────────────────────────────────────────
; Multiboot header (GRUB / multiboot compliant)
; ─────────────────────────────────────────────────────────────────────────────
SECTION .multiboot
align 4
multiboot_header:
    dd 0x1BADB002               ; magic: Multiboot 1
    dd 0x00000005               ; flags: bit 0 (align) + bit 2 (request video mode)
    dd -(0x1BADB002 + 0x00000005) ; checksum

    ; These fields are required when bit 2 is set in flags
    dd 0                        ; header_addr (unused for ELF)
    dd 0                        ; load_addr
    dd 0                        ; load_end_addr
    dd 0                        ; bss_end_addr
    dd 0                        ; entry_addr
    
    ; Graphics fields
    dd 0                        ; mode_type (0 = linear graphics)
    dd 320                      ; width
    dd 200                      ; height
    dd 8                        ; depth (8-bit colors, 256 color palette)

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
    
    ; Call C entry point
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

section .note.GNU-stack noalloc noexec nowrite progbits
