BITS 32
SECTION .multiboot
align 4
    dd 0x1BADB002            ; Magic number
    dd 0x00                  ; Flags
    dd -(0x1BADB002 + 0x00)  ; Checksum

SECTION .text
global _start

_start:
    mov esi, message         ; Point ESI to our string
    mov edi, 0xB8000         ; Point EDI to the VGA text buffer
    mov ah, 0x0F             ; Color attribute: White text on Black background

.print_loop:
    lodsb                    ; Load byte from [ESI] into AL, then increment ESI
    test al, al              ; Check if the character is null (0)
    jz .hang                 ; If null, jump to hang

    mov [edi], ax            ; Write both character (AL) and attribute (AH) to VGA memory
    add edi, 2               ; Move to the next character slot (each takes 2 bytes)
    jmp .print_loop          ; Repeat

.hang:
    cli                      ; Clear interrupts
    hlt                      ; Halt the CPU
    jmp .hang                ; Safety jump

SECTION .data
message db "Hello, World!", 0
