.set ALIGN, 1 << 0 # 0x01 (указывает загрузчику - выравниванивать загружаемые модули по границам страниц)
.set MEMINFO, 1 << 1 # 0x02 (требование о предоставление карты памяти от загрузчика)
.set FLAGS, ALIGN | MEMINFO # (это поле 'флаг' Multiboot)
.set MAGIC, 0xE85250D6 # (магическое число позволяет загрузчику найти заголовок)
.set ARCH, 0 # архитектура (0 = i386)

.section .multiboot
.align 8
multiboot_header_start:
    .long MAGIC
    .long ARCH
    .long multiboot_header_end - multiboot_header_start # HEADER_LENGTH
    .long -(MAGIC + ARCH + (multiboot_header_end - multiboot_header_start)) # CHECKSUM

    # Tag: Framebuffer
    .short 5                   # Type: Framebuffer
    .short 0                   # Flags: 0 = optional, 1 = required
    .long 20                   # Size
    .long 1024                 # Width
    .long 768                  # Height
    .long 32                   # BPP

    .align 8

    # End tag
    .short 0                   # Type: END
    .short 0                   # Flags
    .long 8                    # Size
multiboot_header_end:    

.section .bss
.align 16
stack_bottom:
    .skip 131072  # 128 Kib
stack_top:
    
    
.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp

    # Сохранение мультибутовых регистров в каллер-сейв регистрах
    mov %eax, %esi  # ESI = magic number
    mov %ebx, %edi  # EDI = multiboot info address

    # Инициализация GDT
    # call gdt_init # (в kernel_main)

    # Передача аргументов в kernel_main
    push %edi       # Второй аргумент: адрес информации (uint32_t addr)
    push %esi       # Первый аргумент: магическое число (uint32_t magic)
    call kernel_main

    # Зацикливание, если kernel_main вернется
    cli
1:  hlt
    jmp 1b

.size _start, . - _start
