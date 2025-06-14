.global isr_common_stub

# Макрос для обработчиков без кода ошибки
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    cli
    push $0       # Заглушка для кода ошибки
    push $\num    # Номер прерывания
    jmp isr_common_stub
.endm

# Макрос для обработчиков с кодом ошибки
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    cli
    push $\num    # Номер прерывания
    jmp isr_common_stub
.endm

# Создаем обработчики
ISR_NOERRCODE 0    # Деление на ноль
ISR_NOERRCODE 1    # Отладка
ISR_NOERRCODE 2    # NMI (немаскируемое прерывание)
ISR_NOERRCODE 3    # Breakpoint
ISR_NOERRCODE 4    # Overflow
ISR_NOERRCODE 5    # BOUND Range Exceeded
ISR_NOERRCODE 6    # Invalid Opcode
ISR_NOERRCODE 7    # Device Not Available (No Math Coprocessor)
ISR_ERRCODE   8    # Double Fault
ISR_NOERRCODE 9    # Coprocessor Segment Overrun (устарело)
ISR_ERRCODE   10   # Invalid TSS
ISR_ERRCODE   11   # Segment Not Present
ISR_ERRCODE   12   # Stack-Segment Fault
ISR_ERRCODE   13   # General Protection Fault
ISR_ERRCODE   14   # Page Fault
ISR_NOERRCODE 15   # Reserved
ISR_NOERRCODE 16   # x87 Floating-Point Exception
ISR_ERRCODE   17   # Alignment Check
ISR_NOERRCODE 18   # Machine Check
ISR_NOERRCODE 19   # SIMD Floating-Point Exception
ISR_NOERRCODE 20   # Virtualization Exception
ISR_NOERRCODE 21   # Control Protection Exception
ISR_NOERRCODE 22   # Reserved
ISR_NOERRCODE 23   # Reserved
ISR_NOERRCODE 24   # Reserved
ISR_NOERRCODE 25   # Reserved
ISR_NOERRCODE 26   # Reserved
ISR_NOERRCODE 27   # Reserved
ISR_NOERRCODE 28   # Hypervisor Injection Exception
ISR_NOERRCODE 29   # VMM Communication Exception
ISR_NOERRCODE 30   # Security Exception
ISR_NOERRCODE 31   # Reserved

# Общая часть для всех обработчиков
isr_common_stub:
    pusha          # Сохраняем регистры
    
    mov %ds, %ax
    push %eax
    
    mov $0x10, %ax # Селектор ядра данных
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    # extern isr_handler
    call isr_handler
    
    pop %ebx       # Восстанавливаем сегменты
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    popa
    add $8, %esp   # Чистим код ошибки и номер
    iret


.globl isr128
.type isr128, @function
.extern syscall_handler

isr128:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    # Вызов syscall_handler(number, arg1, arg2, arg3)
    # Аргументы лежат в стеке после pusha и 4 сегментов (4*4 = 16 байт) → 8 регистров * 4 + 16 = 48

    movl %esp, %eax

    pushl 48(%eax)    # arg3
    pushl 48(%eax)    # arg2 (FIXME: повторяется — ниже правильно)
    pushl 44(%eax)    # arg2
    pushl 40(%eax)    # arg1
    pushl 36(%eax)    # syscall number

    call syscall_handler
    addl $20, %esp    # очистить стек (5 аргументов)

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    iret


/* Макрос для обработки IRQ */
.macro IRQ num
    .globl irq\num
    irq\num:
        pushl $0              /* Фиктивный код ошибки */
        pushl $\num + 32      /* Номер прерывания (IRQ0 = 32, IRQ1 = 33, ...) */
        jmp irq_common
.endm

/* Генерация обработчиков для IRQ0-IRQ15 */
IRQ 0   /* Таймер */
IRQ 1   /* Клавиатура */
IRQ 2   /* Каскадирование PIC */
IRQ 3   /* COM2 */
IRQ 4   /* COM1 */
IRQ 5   /* LPT2 */
IRQ 6   /* Floppy */
IRQ 7   /* LPT1 */
IRQ 8   /* RTC */
IRQ 9   /* Free */
IRQ 10  /* Free */
IRQ 11  /* Free */
IRQ 12  /* PS/2 мышь */
IRQ 13  /* FPU */
IRQ 14  /* IDE0 */
IRQ 15  /* IDE1 */

/* Общий обработчик для IRQ */
irq_common:
    pushal               /* Сохраняем регистры */
    mov %ds, %ax
    pushl %eax           /* Сохраняем DS */

    mov $0x10, %ax       /* Селектор данных ядра */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call irq_handler     /* Вызов C-обработчика */

    popl %eax            /* Восстанавливаем DS */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popal                /* Восстанавливаем регистры */
    addl $8, %esp        /* Чистим номер прерывания и код ошибки */
    iret    
