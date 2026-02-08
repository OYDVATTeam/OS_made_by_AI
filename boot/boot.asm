; boot.asm
BITS 32

; ───────────────────────────────────────────────
; Multiboot header (GRUB / multiboot compliant)
; ───────────────────────────────────────────────
SECTION .multiboot
align 4
multiboot_header:
    dd 0x1BADB002             ; magic
    dd 0x00                   ; flags (no special requests)
    dd -(0x1BADB002 + 0x00)   ; checksum

; ───────────────────────────────────────────────
; Entry point – called by bootloader
; ───────────────────────────────────────────────
SECTION .text
global _start
extern kernel_main            ; defined in C file

_start:
    ; ── Very minimal setup ───────────────────────
    cli                       ; disable interrupts (optional at this stage)

    ; Set up a basic stack (very important!)
    mov esp, stack_top

    ; You may want to zero .bss here, save multiboot info, etc.
    ; For now we keep it ultra-minimal

    ; Call C entry point
    call kernel_main

    ; If kernel_main returns (which it shouldn't), infinite loop
.hang:
    hlt
    jmp .hang

; ───────────────────────────────────────────────
; Small stack (can be much larger in .bss later)
; ───────────────────────────────────────────────
SECTION .bss
align 16
stack_bottom:
    resb 16384                ; 16 KiB stack — enough for start
stack_top:
