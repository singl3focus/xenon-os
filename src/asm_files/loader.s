global loader
extern kmain
extern bga_init

[BITS 32]

; ---- Multiboot2 header ----
section .multiboot progbits alloc
align 8
multiboot_header_start:
    dd 0xE85250D6   ; magic (Multiboot2)
    dd 0            ; architecture = i386
    dd multiboot_header_end - multiboot_header_start
    dd -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header_start))
multiboot_header_end:

; ---- Секция BSS ----
section .bss
align 16
kernel_stack:
    resb 4096

; ------------------------
; GDT
; ------------------------
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

; ------------------------
; TEXT (loader)
; ------------------------
section .text
loader:
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:flush_cs

flush_cs:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Установим стек
    mov esp, kernel_stack + 4096

    ; Инициализация BGA
    call bga_init

    ; Вызов ядра
    call kmain

hang:
    hlt
    jmp hang
