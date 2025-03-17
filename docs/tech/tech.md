# Описание 

- Сборка .c файлов: /
```gcc -m32 -O0 -nostdlib -nostdinc -ffreestanding -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -c kmain.c -o kmain.o```

- Сборка загручика: /
```nasm -f elf32 loader.s```

- Сборка ядра: /
```ld -melf_i386 -T link.ld loader.o kmain.o -o kernel.elf```

- Сборка образа ОС: /
```genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso```

- Запуск образа через Bochs (рекомендуется 3.0 и выше): /
``````
