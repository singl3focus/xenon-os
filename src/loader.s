global loader                   ; the entry symbol for ELF

MAGIC_NUMBER equ 0x1BADB002     ; define the magic number constant
FLAGS        equ 0x0            ; multiboot flags
CHECKSUM     equ -MAGIC_NUMBER  ; calculate the checksum
                                ; (magic number + checksum + flags should equal 0)

; bss только для резервирования памяти

section .bss
align 4                     ; Выравнивание по 4 байта
kernel_stack:               ; Метка начала стека
    resb 4096               ; Резервируем 4096 байт (4 КБ) для стека

; text - Исполняемый код должен находиться в секции

section .text:                  ; start of the text (code) section
align 4                         ; the code must be 4 byte aligned
    dd MAGIC_NUMBER             ; write the magic number to the machine code,
    dd FLAGS                    ; the flags,
    dd CHECKSUM                 ; and the checksum

loader:                         ; the loader label (defined as entry point in linker script)
    mov esp, kernel_stack + 4096 ; Устанавливаем указатель стека
    mov eax, 0xCAFEBABE          ; 
    extern kmain                 ; Объявляем функцию kmain из C
    call kmain                   ; Вызываем kmain
.loop:
    jmp .loop                    ; Бесконечный цикл (на всякий случай)
