BITS 32

SECTION .multiboot
align 4
    dd 0x1BADB002             ; magic
    dd 0x00                   ; flags
    dd -(0x1BADB002 + 0x00)   ; checksum

SECTION .text
global _start

_start:
    ; ───────────────────────────────────────────────
    ; Simple VGA text-mode ASCII animation
    ; ───────────────────────────────────────────────

    mov edi, 0xB8000          ; VGA text buffer start
    mov ah, 0x0F              ; attribute: white on black

    ; We'll cycle these 4 frames
animation_loop:
    mov esi, frame1
    call print_frame

    mov esi, frame2
    call print_frame

    mov esi, frame3
    call print_frame

    mov esi, frame4
    call print_frame

    jmp animation_loop        ; forever

; ───────────────────────────────────────────────
; Helper: print one frame + short delay
; ───────────────────────────────────────────────
print_frame:
    push edi
    push esi
    push ecx

    mov ecx, 0xB8FA0          ; ~rough delay counter (~0.1–0.3s depending on CPU)
.delay:
    loop .delay

    ; Clear previous text area (16 chars wide is enough)
    mov ebx, edi
    mov cl, 32                ; 16 chars × 2 bytes
.clear:
    mov word [ebx], 0x0F20    ; space + white-on-black
    add ebx, 2
    dec cl
    jnz .clear

    ; Print current frame
    mov ebx, edi              ; start position
.print_char:
    lodsb                     ; al = next char
    test al, al
    jz .done
    mov [ebx], ax             ; char + attribute
    add ebx, 2
    jmp .print_char
.done:
    pop ecx
    pop esi
    pop edi
    ret

; ───────────────────────────────────────────────
; Animation frames (keep them same length!)
; ───────────────────────────────────────────────
SECTION .data

frame1  db "Hello >",0
frame2  db "Hello -",0
frame3  db "Hello <",0
frame4  db "Hello |",0

; ───────────────────────────────────────────────
; Alternative style – pulsing + spinner (uncomment to use)
; ───────────────────────────────────────────────
; frame1  db "H e l l o   ",0
; frame2  db " H e l l o  ",0
; frame3  db "  H e l l o ",0
; frame4  db "   H e l l o",0
