; boot.asm - minimal MBR: читает 4 сектора (LBA 1..4) в 0x0000:0x8000 и прыгает туда
org 0x7C00

BITS 16

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    sti

    ; Используем BIOS int13h CHS (simple) — но при создании образа с qemu/dd LBA 1..4 будут доступны как секторы 1..4.
    ; Адрес загрузки: 0x0000:0x8000
    mov bx, 0x8000
    mov dh, 0        ; head
    mov ch, 0        ; track
    mov cl, 2        ; sector (сектор 1 = MBR, читаем начиная с 2 -> cl=2)
    mov al, 4        ; количество секторов
    mov dl, [boot_drive]
    mov ah, 0x02
    int 0x13
    jc disk_error

    ; Перейти в Stage2
    jmp 0x0000:0x8000

disk_error:
    ; простая ошибка: печатаем 'E' и зависаем
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    cli
    hlt
    jmp $

; сохраняем значение загрузочного диска (BIOS DL при старте)
boot_drive db 0

; Заполнить до 510 байт нулями и сигнатуру
times 510-($-$$) db 0
dw 0xAA55

