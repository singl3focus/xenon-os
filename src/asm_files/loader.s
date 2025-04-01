global loader                   ; the entry symbol for ELF
extern kmain

; Multiboot header constants
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002  ; magic number for Multiboot
MULTIBOOT_HEADER_FLAGS equ 0x00000003  ; flags: align modules on page boundaries
                                       ; and provide memory map
MULTIBOOT_HEADER_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

KERNEL_STACK_SIZE equ 4096      ; size of stack in bytes

section .multiboot              ; Multiboot header section
align 4                         ; align at 4 bytes
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC   ; magic number
    dd MULTIBOOT_HEADER_FLAGS   ; flags
    dd MULTIBOOT_HEADER_CHECKSUM ; checksum

section .bss
align 4				; align at 4 bytes
kernel_stack:			; label points to beginning of memory
    resb KERNEL_STACK_SIZE	; reserve stack for the kernel

section .data
; Нулевой дескриптор (обязателен)
gdt_start:
    dq 0x0000000000000000  ; 8 нулевых байт

; Кодовый сегмент (0x08)
gdt_code:
    dw 0xFFFF      ; Limit (0-15)
    dw 0x0000      ; Base (0-15)
    db 0x00        ; Base (16-23)
    db 0x9A        ; Access Byte (P=1, DPL=0, S=1, Type=1010)
    db 0xCF        ; Flags (G=1, D/B=1, L=0, AVL=0) + Limit (16-19)
    db 0x00        ; Base (24-31)

; Сегмент данных (0x10)
gdt_data:
    dw 0xFFFF      ; Limit (0-15)
    dw 0x0000      ; Base (0-15)
    db 0x00        ; Base (16-23)
    db 0x92        ; Access Byte (P=1, DPL=0, S=1, Type=0010)
    db 0xCF        ; Flags (G=1, D/B=1, L=0, AVL=0) + Limit (16-19)
    db 0x00        ; Base (24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Размер GDT (3*8 -1 = 23)
    dd gdt_start                ; Адрес GDT

section .text:                  ; start of the text (code) section
loader:                         ; the loader label (defined as entry point in linker script)
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:flush_cs

flush_cs:
    mov ax, 0x10 ; Селектор данных (0x10)
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Тестирование доступа к памяти
    mov dword [0x1000], 0x12345678  ; Запись значения по адресу 0x1000
    mov ebx, [0x1000]               ; Чтение значения обратно
    cmp ebx, 0x12345678             ; Проверка, что значение совпадает
    je memory_access_ok             ; Если совпадает, переход к метке

    ; Если значение не совпадает, остановить процессор
    hlt

memory_access_ok:
    ; Продолжение выполнения

    mov esp, kernel_stack + KERNEL_STACK_SIZE  ; Инициализация стека
    call kmain                                 ; Вызов основной функции
    hlt                                        ; Остановка процессора
