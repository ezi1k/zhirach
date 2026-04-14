; kernel.asm - raw binary, точка входа в начале
org 0x0000
BITS 16

start:
    ; печать строки
    mov si, msg
.print:
    lodsb
    cmp al, 0
    je .hang
    mov ah, 0x0E
    int 0x10
    jmp .print

.hang:
    cli
    hlt
    jmp $

msg db 'Hello from kernel (raw)!',0
times 512*10-($-$$) db 0    ; сделать kernel размером несколько секторов (10 секторов)

