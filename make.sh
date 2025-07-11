#!/bin/bash

set -e # Прерывать выполнение при ошибках

OS_NAME="xenon-os"

# Пути к инструментам
GCC="i686-elf-gcc"
ASM="i686-elf-as"
LINKER="i686-elf-gcc"
GRUB="grub-file"
GRUB_BUILDER="grub-mkrescue"

# Директории проекта
KERNEL_DIR="kernel"
ARCH_DIR="${KERNEL_DIR}/arch/i386"
LIBC_DIR="libc"
BUILD_DIR="build"
ISO_DIR="iso"

# Очистка предыдущей сборки
echo "Очистка предыдущей сборки..."
rm -rf ${BUILD_DIR} ${ISO_DIR}
mkdir -p ${BUILD_DIR}

# Пути к заголовочным файлам
INCLUDES="-I${KERNEL_DIR}/include -I${KERNEL_DIR}/include/drivers -I${LIBC_DIR}/include -I${ARCH_DIR}"

# Общие флаги компиляции
CFLAGS="-std=gnu99 -ffreestanding -m32 -O2 -Wall -Wextra -nostartfiles -g ${INCLUDES}"

# Компиляция ассемблерных файлов
echo "Компиляция ассемблерных файлов..."
$ASM ${ARCH_DIR}/boot.S -o ${BUILD_DIR}/boot.o
$ASM ${ARCH_DIR}/io.S -o ${BUILD_DIR}/io.o
$ASM ${ARCH_DIR}/gdt_flush.S -o ${BUILD_DIR}/gdt_flush.o
$ASM ${ARCH_DIR}/idt_load.S -o ${BUILD_DIR}/idt_load.o
$ASM ${ARCH_DIR}/interrupts.S -o ${BUILD_DIR}/interrupts.o

# Компиляция ядра и драйверов
echo "Компиляция ядра и драйверов..."
KERNEL_SOURCES=(
    "${KERNEL_DIR}/kernel/kernel.c"
    "${ARCH_DIR}/serial.c"
    "${ARCH_DIR}/tty.c"
    "${ARCH_DIR}/gdt.c"
    "${ARCH_DIR}/idt.c"
    "${ARCH_DIR}/irq.c"
    "${ARCH_DIR}/isr.c"
    "${ARCH_DIR}/keyboard.c"
    "${ARCH_DIR}/pic.c"
    "${ARCH_DIR}/draw_logo.c"
    "${ARCH_DIR}/fb.c"
    "${ARCH_DIR}/font.c"
    "${ARCH_DIR}/multiboot2.c"
    "${ARCH_DIR}/timer.c"
    "${ARCH_DIR}/ata.c"
    "${ARCH_DIR}/fat16.c"
    "${ARCH_DIR}/shell.c"
    "${ARCH_DIR}/syscall.c"
)

for source in "${KERNEL_SOURCES[@]}"; do
    obj="${BUILD_DIR}/$(basename ${source%.*}).o"
    $GCC ${CFLAGS} -c ${source} -o ${obj}
done

# Компиляция стандартной библиотеки (libc)
echo "Компиляция стандартной библиотеки..."
LIBC_SOURCES=(
    "${LIBC_DIR}/stdio/printf.c"
    "${LIBC_DIR}/stdio/putchar.c"
    "${LIBC_DIR}/stdio/puts.c"
    "${LIBC_DIR}/stdlib/abort.c"
    "${LIBC_DIR}/stdlib/itoa.c"
    "${LIBC_DIR}/string/memcmp.c"
    "${LIBC_DIR}/string/memcpy.c"
    "${LIBC_DIR}/string/memmove.c"
    "${LIBC_DIR}/string/memset.c"
    "${LIBC_DIR}/string/strcasecmp.c"
    "${LIBC_DIR}/string/strcat.c"
    "${LIBC_DIR}/string/strlen.c"
    "${LIBC_DIR}/string/strcpy.c"
    "${LIBC_DIR}/string/strncat.c"
    "${LIBC_DIR}/string/strcmp.c"
    "${LIBC_DIR}/string/strpbrk.c"
    "${LIBC_DIR}/string/strspn.c"
    "${LIBC_DIR}/string/strtok.c"
)

for source in "${LIBC_SOURCES[@]}"; do
    obj="${BUILD_DIR}/$(basename ${source%.*}).o"
    $GCC ${CFLAGS} -c ${source} -o ${obj}
done

# Линковка
echo "Линковка..."
$LINKER -T ${ARCH_DIR}/linker.ld -o ${BUILD_DIR}/${OS_NAME}.bin \
    -ffreestanding -O2 -nostdlib \
    ${BUILD_DIR}/boot.o \
    ${BUILD_DIR}/io.o \
    ${BUILD_DIR}/gdt_flush.o \
    ${BUILD_DIR}/idt_load.o \
    ${BUILD_DIR}/interrupts.o \
    ${BUILD_DIR}/kernel.o \
    ${BUILD_DIR}/serial.o \
    ${BUILD_DIR}/tty.o \
    ${BUILD_DIR}/gdt.o \
    ${BUILD_DIR}/idt.o \
    ${BUILD_DIR}/irq.o \
    ${BUILD_DIR}/isr.o \
    ${BUILD_DIR}/keyboard.o \
    ${BUILD_DIR}/draw_logo.o \
    ${BUILD_DIR}/fb.o \
    ${BUILD_DIR}/font.o \
    ${BUILD_DIR}/pic.o \
    ${BUILD_DIR}/multiboot2.o \
    ${BUILD_DIR}/timer.o \
    ${BUILD_DIR}/ata.o \
    ${BUILD_DIR}/fat16.o \
    ${BUILD_DIR}/shell.o \
    ${BUILD_DIR}/syscall.o \
    ${BUILD_DIR}/printf.o \
    ${BUILD_DIR}/putchar.o \
    ${BUILD_DIR}/puts.o \
    ${BUILD_DIR}/abort.o \
    ${BUILD_DIR}/itoa.o \
    ${BUILD_DIR}/memcmp.o \
    ${BUILD_DIR}/memcpy.o \
    ${BUILD_DIR}/memmove.o \
    ${BUILD_DIR}/memset.o \
    ${BUILD_DIR}/strcasecmp.o \
    ${BUILD_DIR}/strcat.o \
    ${BUILD_DIR}/strlen.o \
    ${BUILD_DIR}/strcpy.o \
    ${BUILD_DIR}/strncat.o \
    ${BUILD_DIR}/strcmp.o \
    ${BUILD_DIR}/strpbrk.o \
    ${BUILD_DIR}/strspn.o \
    ${BUILD_DIR}/strtok.o \
    -lgcc

# Генерация ELF-файла с отладочной информацией
echo "Генерация ELF-файла с отладочной информацией..."
cp ${BUILD_DIR}/${OS_NAME}.bin ${BUILD_DIR}/${OS_NAME}.elf

# Проверка Multiboot
echo "Проверка Multiboot..."
if $GRUB --is-x86-multiboot2 ${BUILD_DIR}/${OS_NAME}.bin; then
    echo "Multiboot ELF валиден"
else
    echo "ОШИБКА: ELF файл не соответствует стандарту Multiboot!"
    exit 1
fi

# Подготовка ISO
echo "Подготовка ISO..."
mkdir -p ${ISO_DIR}/boot/grub
cp ${BUILD_DIR}/${OS_NAME}.bin ${ISO_DIR}/boot/
cp configs/grub.cfg ${ISO_DIR}/boot/grub/

echo "Сборка ISO..."
${GRUB_BUILDER} -o ${OS_NAME}.iso ${ISO_DIR}

echo "Сборка успешно завершена! Образ: xenon-os.iso"