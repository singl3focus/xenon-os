#!/bin/bash
set -e # Прерывать выполнение при ошибках

# source ./make_clear.sh

: '
-m32: Генерировать 32-битный код (x86).

-nostdlib: Не использовать стандартную системную библиотеку запуска/остановки (crt0.o) и libc.

-nostdinc: Не искать заголовочные файлы (#include <...>) в стандартных системных директориях.

-fno-builtin: Отключить встроенные оптимизации компилятора для стандартных функций (если мы сами реализуем memcpy, компилятор не должен подменять её своей версией).

-fno-stack-protector: Отключить защиту стека (Canaries). В ранней ОС она не реализована и будет вызывать ошибки.

-nostartfiles: Не использовать стандартные файлы запуска (вроде crt0.o), которые вызывают main.
Мы сами инициализируем стек и вызываем kmain.

-nodefaultlibs: Явно отключить все стандартные библиотеки по умолчанию (дополнение к -nostdlib).

-Wall -Wextra -Werror: Включить все предупреждения, дополнительные предупреждения и
трактовать любое предупреждение как ошибку компиляции. Критически важно для написания надежного низкоуровневого кода.

-c: Только компилировать в объектный файл (.o), не линковать.
'

# Пути к инструментам
GCC="i686-elf-gcc"
ASM="i686-elf-as"
LINKER="i686-elf-gcc"
GRUB="grub-file"
GRUB_BUILDER="grub-mkrescue"

echo "Компиляция загрузочного кода..."
$ASM boot.s -o boot.o

echo "Компиляция ядра..."
$GCC -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -m32 -O2 -Wall -Wextra -Werror -nostartfiles

echo "Линковка..."
$LINKER -T linker.ld -o xenon-os.bin -ffreestanding -O2 -nostdlib -no-pie boot.o kernel.o -lgcc

echo "Проверка Multiboot..."
if $GRUB --is-x86-multiboot xenon-os.bin; then
  echo "Multiboot-совместимость подтверждена"
else
  echo "ОШИБКА: Файл не соответствует стандарту Multiboot!"
  exit 1
fi

# Создание структуры каталогов
mkdir -p iso/boot/grub
mv xenon-os.bin iso/boot/
cp configs/grub.cfg iso/boot/grub/

echo "Сборка ISO..."
${GRUB_BUILDER} -o xenon-os.iso iso/ # 2>make_log.txt

echo "Сборка успешно завершена!"