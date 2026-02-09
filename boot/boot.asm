; boot.asm
BITS 32

; ─────────────────────────────────────────────────────────────────────────────
; Multiboot header (GRUB / multiboot compliant)
; ─────────────────────────────────────────────────────────────────────────────
SECTION .multiboot
align 4
multiboot_header:
    dd 0x1BADB002               ; magic
    dd 0x00000003               ; flags: bits 0 and 1 (Align + Mem Info)
    dd -(0x1BADB002 + 0x00000003) ; checksum
    ; Remove the DD 0, 320, 200, 8 etc. fields entirely

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
