BITS 32
SECTION .multiboot
align 4
    dd 0x1BADB002              ; Magic number
    dd 0x00                    ; Flags
    dd -(0x1BADB002 + 0x00)    ; Checksum

SECTION .text
global _start

_start:
    mov esi, message
    mov edi, 0xB8000           ; VGA Text Buffer
    mov ah, 0x0F               ; White on Black

.print:
    lodsb
    test al, al
    jz .hang
    mov [edi], ax
    add edi, 2
    jmp .print

.hang:
    cli
    hlt
    jmp .hang

SECTION .data
message db "Hello, World!", 0
