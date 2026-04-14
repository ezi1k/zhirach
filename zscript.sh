# Исправленная функция записи 4 байт (LE)
write_le32() {
    local v=$1
    # Используем printf для вывода байтов в шестнадцатеричном виде
    printf "\\x$(printf "%02x" $((v & 0xFF)))" >> "$HEADER_BIN"
    printf "\\x$(printf "%02x" $(( (v >> 8) & 0xFF )))" >> "$HEADER_BIN"
    printf "\\x$(printf "%02x" $(( (v >> 16) & 0xFF )))" >> "$HEADER_BIN"
    printf "\\x$(printf "%02x" $(( (v >> 24) & 0xFF )))" >> "$HEADER_BIN"
}

# Формирование заголовка (LBA 5)
printf 'KRNL' > "$HEADER_BIN"
write_le32 $LOAD_ADDR
write_le32 $KERNEL_SIZE_BYTES
write_le32 $ENTRY

# Добиваем заголовок до размера сектора (512 байт), чтобы dd не обрезал данные
truncate -s 512 "$HEADER_BIN"

# Запись в образ
dd if="$MBR_BIN" of="$OUT" conv=notrunc bs=512 count=1
dd if="$STAGE2_PAD" of="$OUT" conv=notrunc bs=512 seek=1
dd if="$HEADER_BIN" of="$OUT" conv=notrunc bs=512 seek=5
dd if="$KERNEL_BIN" of="$OUT" conv=notrunc bs=512 seek=6

