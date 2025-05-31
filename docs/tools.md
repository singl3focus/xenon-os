# Technologies
 
Небольшое рукодство по стеку используемых технологий при разработке ОС: ASM, C, LD и др. Содержит в себе, как теоритеческую и практическую базу, так и некоторые нестадартные приёмы, использующиеся в разработке. 

## ASM

Для разработки, мы используем NASM. Это диалект ассемблера специально разработанный под процессоры семейства Intel x86.
- ОС работает в защищенном 32-битном режиме. Поэтому, по умолчанию ассемблер в такой ОС будет производить 32-битный код, что соответсвует нашей архитектуре.
- Блок ```.text```: раздел содержит фактические машинные инструкции, составляющие программу.
- ```.globl _start```: .global <символ> делает символ видимым для компоновщика. При определении символа в подпрограмме его значение становится доступным для других связанных подпрограмм. Иначе говоря, символ получает атрибуты от символа с таким же именем, находящегося в другом файле, связанном с этой программой.

Ассемблер играет критическую роль в создании ОС для x86, особенно на этапах инициализации, работы с аппаратурой и обработки прерываний. Рассмотрим основные особенности и инструменты.

1. Синтаксис NASM
NASM (Netwide Assembler) — ассемблер с синтаксисом Intel, популярный в разработке ОС благодаря:

Поддержке 32/64-битных инструкций.

Гибкости в определении сегментов и меток.

Возможности генерации плоских бинарных файлов (для загрузчиков).

Пример структуры программы:

```nasm
; Пример кода ядра ОС
BITS 32             ; Генерировать 32-битный код
SECTION .text       ; Секция исполняемого кода
GLOBAL _start       ; Экспорт точки входа`

_start:
    mov eax, 0x1234 ; Пример инструкции
    jmp $           ; Бесконечный цикл
```

2. Основные директивы NASM
```BITS 32``` — указывает, что код предназначен для 32-битного режима.

```ORG 0x7C00``` — задает смещение загрузчика в памяти (для реального режима).

```SECTION``` — определяет секции (.text, .data, .bss).

```GLOBAL / EXTERN``` — управление видимостью символов для линкера.

```RESB, RESW, RESD``` — резервирование неинициализированной памяти (аналог char buffer[100] в C).

Пример секции данных:

```nasm
SECTION .data
msg db 'Hello, OS World!', 0   ; Строка с нулевым терминатором

SECTION .bss
buffer resb 256                ; Буфер на 256 байт
```

3. Работа с защищенным режимом
Для перехода в 32-битный защищенный режим требуется:

Отключить прерывания (cli).

Загрузить GDT (Global Descriptor Table):
```nasm
lgdt [gdt_descriptor]  ; Загрузить регистр GDTR
```

Установить бит PE в CR0:
```nasm
mov eax, cr0
or  eax, 0x1
mov cr0, eax
```

Перейти в защищенный режим через дальний jump:
```nasm
jmp CODE_SEG:protected_mode_start
```

Пример GDT:
```nasm
gdt_start:
    dq 0x0                ; Нулевой дескриптор
gdt_code:
    dw 0xFFFF             ; Лимит (0-15)
    dw 0x0                ; База (0-15)
    db 0x0                ; База (16-23)
    db 0b10011010         ; P=1, DPL=0, Code, Exec/Read
    db 0b11001111         ; G=1, D=1, Лимит (16-19)
    db 0x0                ; База (24-31)
gdt_data:
    ; Аналогично, но для данных (Read/Write)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Размер GDT
    dd gdt_start                ; Адрес GDT
```

4. Взаимодействие с оборудованием
Порты ввода-вывода:
```nasm
in al, 0x60      ; Чтение из порта клавиатуры
out 0x20, al     ; Отправка EOI в PIC
```

Настройка контроллера прерываний (PIC):
```nasm
; Перенастройка IRQ0-IRQ7 на вектора 32-39
mov al, 0x11
out 0x20, al     ; Инициализация ведущего PIC
out 0xA0, al     ; Инициализация ведомого PIC
```

Обработка прерываний:
```nasm
; ISR для исключения деления на ноль (вектор 0)
isr0:
    pusha
    ; ... обработка ...
    popa
    iret
```

5. Интеграция с кодом на C
Соглашение о вызовах:

Параметры передаются через стек (в порядке справа налево).

Возвращаемое значение — в EAX.

Сохраняемые регистры: EBX, ESI, EDI, EBP.

Пример вызова функции на C из ассемблера:

```nasm
extern kernel_main  ; Объявление внешней функции

_start:
    call kernel_main  ; Вызов функции main ядра
```

Пример функции на ассемблере, вызываемой из C:
```nasm
GLOBAL read_port
read_port:
    mov edx, [esp + 4]  ; Порт
    in  al, dx
    ret
```

6. Линковка и структура образа
Скрипт линкера: Определяет расположение секций в памяти.

```ld
ENTRY(_start)                ; Точка входа
SECTIONS {
    . = 0x100000;            ; Адрес загрузки ядра
    .text : { *(.text) }     ; Код
    .data : { *(.data) }     ; Данные
    .bss  : { *(.bss) }      ; Неинициализированные данные
}
```

Генерация образа:

```bash
nasm -f elf32 kernel.asm -o kernel.o
ld -m elf_i386 -T linker.ld kernel.o -o kernel.bin
```

7. Отладка и эмуляция
Инструменты:

QEMU: Эмуляция системы (qemu-system-i386 -kernel kernel.bin).

GDB: Отладка через -s -S флаги QEMU.

Bochs: Альтернативный эмулятор с встроенным отладчиком.

Логирование через последовательный порт:

```nasm
; Отправка символа в COM1 (порт 0x3F8)
serial_write:
    mov dx, 0x3F8
    out dx, al
    ret
```

8. Получение информации об объектом файле:

```bash
objdump -h kernel.elf

kernel.elf:     file format elf32-i386

Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .multiboot    0000000c  00100000  00100000  00001000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
```                  

## Подготовка необходимых инструментов

### GNU tools

```bash
export TARGET=i686-elf
export PREFIX=/usr/local/i686-elf
```

```bash
wget https://ftp.gnu.org/gnu/binutils/binutils-2.44.tar.gz
tar -xf binutils-2.44.tar.gz
```

```bash
./binutils-2.44/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror
```

```bash
make
make install

ls $PREFIX/bin/i686-elf-*
# Должны быть файлы: ar, as, ld, objdump и др.
```

```bash
sudo apt install build-essential flex bison libgmp-dev libmpfr-dev libmpc-dev texinfo
```

```bash
wget https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
tar -xf gcc-13.2.0.tar.gz
```

```bash
./gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
```

```bash
make all-gcc all-target-libgcc
make install-gcc install-target-libgcc

$PREFIX/bin/i686-elf-gcc --version
# Выовод должен быть таким: i686-elf-gcc (GCC) 13.2.0
```

- Добавьте созданные инструменты в $PATH, чтобы их можно было использовать по названию (без указания полного пути):
```bash
nano ~/.bashrc # в конец файла добавить export PATH=$PATH:/usr/local/i686-elf/bin
source ~/.bashrc # перезагрузить конфигурацию
echo $PATH # вывод должен содержать /usr/local/i686-elf/bin
```

### GRUB

```bash
git clone https://github.com/vertis/objconv.git
cd ./objconv/
```

fix it:
1. src/coff.h
```cpp
// Before:
#define PE_SCN_MEM_WRITE        0x80000000

// After:
#define PE_SCN_MEM_WRITE        0x80000000U  // Add suffix U for unsigned
```

2. src/disasm1.cpp
```cpp
// Before:
SFunctionRecord fun = {Section, 0, Sections[Section].TotalSize, 0, 0};

// After:
SFunctionRecord fun = {static_cast<int32>(Section), 0, Sections[Section].TotalSize, 0, 0};
```

3. src/disasm2.cpp
```cpp
// Before:
{0x80000000, "MVEX option bits not allowed here"}

// After:
{0x80000000U, "MVEX option bits not allowed here"}  // Добавляем U
```

4. src/library.cpp
```cpp
// Before:
sprintf(SymTab.FileSize, "%u", Index3Size);

// After:
snprintf(SymTab.FileSize, sizeof(SymTab.FileSize), "%u", Index3Size);  // Безопасная версия
```

we use g++ version 13.3.0

```bash
g++ -o objconv -O2 src/*.cpp -Wno-narrowing
cd ..
```

```bash
git clone --recurse-submodules --depth 1 git://git.savannah.gnu.org/grub.git
cd grub
```

```bash
./bootstrap # или ./autogen.sh

mkdir ../build-grub
cd ../build-grub

../grub/configure \
  --disable-werror \
  --prefix=$HOME/opt/grub \
  TARGET_CC=$TARGET-gcc \
  TARGET_OBJCOPY=$TARGET-objcopy \
  TARGET_STRIP=$TARGET-strip \
  TARGET_NM=$TARGET-nm \
  TARGET_RANLIB=$TARGET-ranlib \
  --target=$TARGET
```

```bash
sudo apt install gawk, xorriso, mtools
```

```bash
sudo update-alternatives --set awk /usr/bin/gawk

make
make install
```

- Аналогично устанавливаем в PATH: ```/home/focus/opt/grub/sbin```, ```/home/focus/opt/grub/bin```