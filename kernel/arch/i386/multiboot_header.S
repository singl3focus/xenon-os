.set ALIGN, 1 << 0 # 0x01 (указывает загрузчику - выравниванивать загружаемые модули по границам страниц)
.set MEMINFO, 1 << 1 # 0x02 (требование о предоставление карты памяти от загрузчика)
.set FLAGS, ALIGN | MEMINFO # (это поле 'флаг' Multiboot)
.set MAGIC, 0xE85250D6 # (магическое число позволяет загрузчику найти заголовок)
.set CHECKSUM, -(MAGIC + FLAGS) # (сумма, чтобы доказать, что мы мультизагрузочные) 

.section .multiboot 
.align 8
multiboot_header_start:
    .long MAGIC
    .long 0
    .long multiboot_header_end - multiboot_header_start
    .long -(MAGIC + 0 + (multiboot_header_end - multiboot_header_start))
multiboot_header_end:
    