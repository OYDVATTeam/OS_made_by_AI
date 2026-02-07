; boot.asm
; Simple bootloader that prints "Hello, World!"

BITS 16
ORG 0x7C00

start:
    mov si, message      ; Point SI to the message

print_loop:
    lodsb                ; Load byte at SI into AL, increment SI
    cmp al, 0
    je hang               ; If null terminator, stop
    mov ah, 0x0E          ; BIOS teletype output
    int 0x10
    jmp print_loop

hang:
    cli
    hlt

message:
    db "Hello, World!", 0

; Pad to 510 bytes
times 510 - ($ - $$) db 0

; Boot signature
dw 0xAA55
