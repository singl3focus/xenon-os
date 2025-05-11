global loader                   ; the entry symbol for ELF

enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Multiboot header constants
MULTIBOOT_HEADER_MAGIC equ 0x1BADB002  ; magic number for Multiboot
MULTIBOOT_HEADER_FLAGS equ 0x00000003  ; flags: align modules on page boundaries
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
; GDT
align 4
gdt_start: 
    dq 0x0000000000000000  ; Нулевой дескриптор
gdt_code: 
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (0-15)
    db 0x00                ; Base (16-23)
    db 0x9A                ; Access Byte
    db 0xCF                ; Flags + Limit
    db 0x00                ; Base (24-31)
gdt_data: 
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (0-15)
    db 0x00                ; Base (16-23)
    db 0x92                ; Access Byte
    db 0xCF                ; Flags + Limit
    db 0x00                ; Base (24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Размер GDT
    dd gdt_start - 0xC0000000; Физический адрес GDT

section .data
align 4096
pdt:
    ; Identity mapping (0x00000000 -> 0x00000000)
    dd 0x00000083 | (0 << 22)
    times 768 - 1 dd 0
    ; Higher-half: 0xC0000000 -> 0x100000 (1 МБ)
    dd 0x00100083 | (0x40 << 22)  ; 0x40 = 0x100000 / 4MB
    times 1024 - 768 - 1 dd 0

section .text:  
loader:                         ; the loader label (defined as entry point in linker script)
    ; Загрузить GDT
    lgdt [gdt_descriptor]

    ; Включить защищённый режим
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:flush_cs  ; Обновить CS

flush_cs:
    ; Обновить сегменты данных
    mov ax, 0x10 ; Селектор сегмента данных
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Настройка пейджинга
    mov eax, pdt - 0xC0000000  ; Физический адрес PDT
    mov cr3, eax

    mov eax, cr4
    or eax, 0x00000010  ; PSE
    mov cr4, eax

    mov eax, cr0
    or eax, 0x80000000  ; PG
    mov cr0, eax

    ;  Переход на виртуальный адрес
    lea ecx, [higher_half] 
    jmp ecx                ; jump to the label

extern kmain

higher_half:
    ; Обновить сегменты данных
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Удалить Identity mapping через физический адрес PDT
    ; mov eax, pdt  ; Физический адрес PDT
    ; mov dword [eax], 0
    ; invlpg [0]

    mov esp, kernel_stack + KERNEL_STACK_SIZE
    call kmain
