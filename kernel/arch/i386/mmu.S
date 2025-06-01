.global switch_page_directory
switch_page_directory:
    push %ebp
    mov %esp, %ebp
    
    mov 8(%ebp), %eax   // Получаем физический адрес PD
    mov %eax, %cr3      // Загружаем в CR3
    
    mov %cr0, %eax
    or $0x80000000, %eax // Устанавливаем бит PG (31)
    mov %eax, %cr0       // Включаем пейджинг
    
    pop %ebp
    ret
    