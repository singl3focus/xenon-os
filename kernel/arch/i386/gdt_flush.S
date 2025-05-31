.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax  # Получить указатель на структуру gdt_ptr
    lgdt (%eax)        # Загрузить GDTR
    
    # Обновить сегментные регистры
    mov $0x10, %ax     # Селектор данных (второй дескриптор: 0x10)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    
    ljmp $0x08, $.flush # Селектор кода (первый дескриптор: 0x08), обновить CS
.flush:
    ret
    