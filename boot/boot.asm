BITS 32
SECTION .multiboot
align 4
    dd 0x1BADB002
    dd 0x0
    dd -(0x1BADB002)

SECTION .text
global _start

_start:
    mov esi, message

.print:
    lodsb
    test al, al
    jz .hang
    mov ah, 0x0E
    int 0x10
    jmp .print

.hang:
    cli
    hlt

message db "Hello, World!", 0
