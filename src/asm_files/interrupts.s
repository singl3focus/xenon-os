[bits 32]

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push %1
    jmp isr_common
%endmacro

; Создаем обработчики для всех исключений
ISR_NOERRCODE 0    ; Divide by Zero
ISR_NOERRCODE 1    ; Debug
; ... пропущенные исключения ...
ISR_ERRCODE 8      ; Double Fault
ISR_ERRCODE 13     ;
; ... продолжаем до 31 ...

; Общий обработчик
isr_common:
    pusha
    mov ax, ds
    push eax        ; Сохраняем DS
    
    mov ax, 0x10    ; Селектор ядра
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    extern isr_handler
    call isr_handler
    
    pop eax         ; Восстанавливаем DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    add esp, 8      ; Чистим номер прерывания и код ошибки
    iret

; Макрос для обработки IRQ
%macro IRQ 1
global irq%1
irq%1:
    push 0              ; Фиктивный код ошибки
    push %1 + 32        ; Номер прерывания (IRQ0 = 32, IRQ1 = 33, ...)
    jmp irq_common
%endmacro

; Генерация обработчиков для IRQ0-IRQ15
IRQ 0   ; Таймер
IRQ 1   ; Клавиатура
IRQ 2   ; Каскадирование PIC
IRQ 3   ; COM2
IRQ 4   ; COM1
IRQ 5   ; LPT2
IRQ 6   ; Floppy
IRQ 7   ; LPT1
IRQ 8   ; RTC
IRQ 9   ; Free
IRQ 10  ; Free
IRQ 11  ; Free
IRQ 12  ; PS/2 мышь
IRQ 13  ; FPU
IRQ 14  ; IDE0
IRQ 15  ; IDE1

; Общий обработчик для IRQ
irq_common:
    pusha               ; Сохраняем регистры
    mov ax, ds
    push eax            ; Сохраняем DS

    mov ax, 0x10        ; Селектор данных ядра
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern irq_handler_c
    call irq_handler_c  ; Вызов C-обработчика

    pop eax             ; Восстанавливаем DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                ; Восстанавливаем регистры
    add esp, 8          ; Чистим номер прерывания и код ошибки
    iret
