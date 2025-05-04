global loader                   ; the entry symbol for ELF
extern kmain
extern vbe_mode_info

; Multiboot header constants
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002  
MULTIBOOT_HEADER_FLAGS equ 0x00000003  
MULTIBOOT_HEADER_CHECKSUM equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

KERNEL_STACK_SIZE equ 4096      

section .multiboot
align 4
multiboot_header:
    dd MULTIBOOT_HEADER_MAGIC   
    dd MULTIBOOT_HEADER_FLAGS   
    dd MULTIBOOT_HEADER_CHECKSUM 

section .bss
align 4
kernel_stack: resb KERNEL_STACK_SIZE  

vbe_mode_info:
    resb 256  

section .data
gdt_start:
    dq 0x0000000000000000  

gdt_code:
    dw 0xFFFF      
    dw 0x0000      
    db 0x00        
    db 0x9A        
    db 0xCF        
    db 0x00        

gdt_data:
    dw 0xFFFF      
    dw 0x0000      
    db 0x00        
    db 0x92        
    db 0xCF        
    db 0x00        

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  
    dd gdt_start                

section .text
loader:
    ; Запрос информации о VBE в реальном режиме
    mov ax, 0x4F01        ; Get Mode Info
    mov cx, 0x117         ; Режим 1024x768x32bpp
    lea di, [vbe_mode_info]
    int 0x10              ; Вызов BIOS

    cmp al, 0x4F          ; Проверка успешности
    jne .vbe_failed       ; Если ошибка — останов

    ; Переход в защищённый режим
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:.flush_cs     ; Фар-джамп для обновления CS

.flush_cs:
    mov ax, 0x10          ; Селектор данных
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Тестирование памяти
    mov dword [0x1000], 0x12345678
    mov ebx, [0x1000]
    cmp ebx, 0x12345678
    je .memory_access_ok

    hlt

.memory_access_ok:
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    call kmain
    hlt

.vbe_failed:
    mov eax, 0xCAFEBABE
    hlt