; stage2.asm - загружен по 0x0000:0x8000
org 0x8000
BITS 16

start_stage2:
    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00    ; стек
    sti

    ; сохранить загрузочный диск (DL пришёл из MBR)
    mov [stage2_boot_drive], dl

    ; адрес заголовка: 0x0000:0x8000 (мы тут)
    ; формат заголовка (по соглашению):
    ; offset 0: magic dword 'KRNL' (0x4B524E4C)
    ; 4: load_addr (4 байта)
    ; 8: size_bytes (4 байта)
    ; 12: entry_offset (4 байта)

    ; Проверка magic
    mov si, header
    mov ax, [si]
    mov dx, [si+2]
    ; сравниваем 4 байта как слово+слово
    ; magic expected: 0x4B52 0x4E4C  ('KR' 'NL') little-endian -> low word 0x524B, high 0x4C4E
    cmp ax, 0x524B
    jne header_fail
    cmp dx, 0x4C4E
    jne header_fail

    ; читаем load_addr, size_bytes, entry_offset
    mov si, header+4
    ; load_addr (dword)
    mov ax, [si]
    mov [load_addr+0], ax
    mov ax, [si+2]
    mov [load_addr+2], ax

    ; size_bytes
    mov si, header+8
    mov ax, [si]
    mov [size_bytes+0], ax
    mov ax, [si+2]
    mov [size_bytes+2], ax

    ; entry_offset
    mov si, header+12
    mov ax, [si]
    mov [entry_offset+0], ax
    mov ax, [si+2]
    mov [entry_offset+2], ax

    ; Для простоты: рассчитываем количество секторов (512 байт)
    ; sectors = (size_bytes + 511) / 512
    ; используем 32‑бит math в 16-bit real — упрощённо: положим, что size < 65536
    mov ax, [size_bytes+0]
    mov cx, 512
    xor dx, dx
    div cx            ; AX = size_bytes / 512, DX = size_bytes % 512
    mov bx, ax
    cmp dx, 0
    je .no_rem
    inc bx
.no_rem:

    ; читаем BX секторов с LBA начиная с 5 (MBR=0, Stage2 сектора 1..4, header+kernel начинаются с LBA 5)
    ; Для простоты используем CHS: читаем по одному секторе в цикле, рассчитывая CHS примитивно для образа QEMU (с 1 head, 63 sectors). 
    ; Чтобы избежать сложностей, предположим образ маленький и использовать int13h функции одиночного сектора с инкрементом CL.
    mov si, kernel_load_seg   ; где загрузим: физический адрес хранится в load_addr (real-mode сегмент:offset)
    ; load_addr как физический 32-bit: разделим на сегмент:offset: положим сегмент = load_addr >> 4, offset = load_addr & 0xF
    ; Но для простоты поддержим адрес 0x00100000 (1MiB) — он не помещается в real-mode сегмент. Поэтому Stage2 должен использовать загрузку через ES:BX с реальным сегментом 0x0000.
    ; Практический упрощённый подход: если load_addr == 0x00100000, читать в 0x0000:0x100000 не реализуемо в real mode напрямую. Мы упростим: предполагаем load_addr < 0x100000 и используем ES:BX.
    ; Поэтому в этом минимальном примере потребуем, чтобы kernel load_addr была < 0xA0000 (обычная real-mode память). Для демонстрации ставим default 0x0000:0x9000.

    ; Получим load_addr low word
    mov ax, [load_addr+0]
    mov bx, ax        ; offset частично
    ; сегмент = load_addr >> 4
    mov ax, [load_addr+2]
    shl ax, 12        ; high word * 65536 -> shift into high dword ; approximate -> плохо, но для простоты: если load_addr fits in 20 bits
    ; Вместо сложных вычислений просто используем заранее заданный сегмент:offset из header: мы договоримся, что load_addr задаётся как segment:offset в header (4 байта сегмент,4 байта offset).
    ; Но чтобы не усложнять, ниже — простая реализация: загружаем в 0x0000:0x9000

    mov ax, 0x0000
    mov es, ax
    mov bx, 0x9000

    ; LBA чтение: читаем по одному сектору, начиная с LBA=5
    mov si, 5        ; lba sector start
.read_loop:
    cmp bx, 0x9000   ; just placeholder, not used
    ; prepare CHS: простая последовательность: cl = (si % 63)+1, ch = (si / 63) % 256, dh = 0
    mov ax, si
    xor dx, dx
    mov cx, 63
    div cx           ; AX = tracks, DX = sectorIndex(0..62)
    inc dx           ; sector number 1..63
    mov ch, al
    mov cl, dl
    mov dh, 0
    mov al, 1        ; read 1 sector
    mov dl, [stage2_boot_drive]
    mov ah, 0x02
    mov bx, 0x9000   ; offset in es
    int 0x13
    jc disk_error2

    add bx, 512
    inc si
    dec [sectors_left]
    cmp byte [sectors_left], 0
    jne .read_loop
    jmp .jump_kernel

disk_error2:
    ; отображаем 'D'
    mov ah, 0x0E
    mov al, 'D'
    int 0x10
    jmp $

header_fail:
    ; печать 'H'
    mov ah, 0x0E
    mov al, 'H'
    int 0x10
    jmp $

.jump_kernel:
    ; простая передача управления: устанавливаем сегменты, стек и прыгаем на 0x0000:0x9000
    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7B00
    sti

    jmp 0x0000:0x9000

; простые переменные и заголовок (здесь header будет в начале Stage2 образа)
header:
    ; Для простоты в этом примерe запишите заголовок вручную в образ kernel.img (см. ниже)
    ; Здесь резервы
    times 16 db 0

stage2_boot_drive db 0
load_addr      dd 0
size_bytes     dd 0
entry_offset   dd 0
sectors_left   db 0

times 510-($-$$) db 0
dw 0x0000

