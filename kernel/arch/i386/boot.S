.section .text
.global _start
.type _start, @function
_start:
    cli

    # mov $temp_stack_top, %esp

    // Сохраняем аргументы Multiboot
    mov %eax, %esi                 // Магическое число
    mov %ebx, %edi                 // Физический адрес информации

    mov $stack_top, %esp
    push %ebx  // Multiboot info
    push %eax  // Multiboot magic
    call kernel_main
    
    # call setup_paging
    # lea higher_half, %ecx
    # jmp *%ecx

higher_half:
    mov $(stack_top - 16), %esp
    and $-16, %esp

    // Преобразуем физический адрес в виртуальный
    add $0xC0000000, %edi
    
    lidt idt_descriptor 

    // Передаем аргументы в kernel_main
    push %edi
    push %esi
    call kernel_main

setup_paging:
    # Создаем временный identity mapping для первых 4MB
    mov $paging_temp_pt, %esi  # Таблица страниц
    mov $0, %edi               # Начальный фрейм
    mov $1024, %ecx            # 1024 записи
    
.fill_pt:
    mov %edi, %eax
    or $0x003, %eax            # Present + R/W
    mov %eax, (%esi)
    add $4, %esi
    add $4096, %edi            # Следующий фрейм
    loop .fill_pt

    # Настраиваем каталог страниц
    mov $paging_temp_pd, %edi
    mov $paging_temp_pt, %eax
    or $0x003, %eax            # Present + R/W
    mov %eax, (%edi)           # Первая запись PD
    mov %eax, 0xC00(%edi)      # Запись для 0xC0000000
    
    # Загружаем PD в CR3
    mov $paging_temp_pd, %eax
    mov %eax, %cr3
    
    # Включаем пейджинг
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0
    
    ret

.section .data
idt_descriptor:
    .word 0x800       // Размер IDT - 1 (2048*8-1)
    .long idt         // Адрес IDT

.section .data
.align 4096
paging_temp_pd:
    .fill 1024, 4, 0  # Page Directory (1024 записей)
paging_temp_pt:
    .fill 1024, 4, 0  # Page Table (1024 записей)



.section .bss
.align 16
# Временный стек (физические адреса)
temp_stack_bottom:
    .skip 4096
temp_stack_top:

# Основной стек (виртуальные адреса)
stack_bottom:
    .skip 16384  # 16 KiB
stack_top:    
    