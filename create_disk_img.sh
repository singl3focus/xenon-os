#!/bin/bash

set -e # Прерывать выполнение при ошибках

# Создаем каталог для монтирования
mkdir -p mnt || true

# Создаем образ диска (64 MiB)
dd if=/dev/zero of=disk.img bs=1M count=64 status=progress

# Создаем раздел
parted disk.img <<EOF
mklabel msdos
mkpart primary fat16 1MiB 100%
set 1 boot on
quit
EOF

# Форматируем раздел (Настройка loop-устройств)
LOOP=$(sudo losetup --find --show --partscan disk.img)
sleep 1 # Даем ядру время распознать разделы

# Форматирование с проверкой
if [ -b "${LOOP}p1" ]; then
    sudo mkfs.vfat -F 16 -n TEST "${LOOP}p1"
else
    echo "Ошибка: раздел не найден"
    exit 1
fi

# Монтирование
sudo mount "${LOOP}p1" mnt

# Работа с файлами
sudo sh -c 'echo "Test file" > mnt/test.txt'

# Очистка
sudo umount mnt
sudo losetup -d "$LOOP"
rm -rf mnt