.set ALIGN, 1 << 0 # 0x01 (указывает загрузчику - выравниванивать загружаемые модули по границам страниц)
.set MEMINFO, 1 << 1 # 0x02 (требование о предоставление карты памяти от загрузчика)
.set FLAGS, ALIGN | MEMINFO # (это поле 'флаг' Multiboot)
.set MAGIC, 0x1BADB002 # (магическое число позволяет загрузчику найти заголовок)
.set CHECKSUM, -(MAGIC + FLAGS) # (сумма, чтобы доказать, что мы мультизагрузочные) 

.section .multiboot
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bss
.align 16
stack_bottom:
    .skip 16384 # 16 Kib
stack_top:
    
    
.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp

    call gdt_init

	call kernel_main

    .size _start, . - _start
