# Компилятор и флаги
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -Iinclude  # Добавьте путь к заголовочным файлам, если нужно

# Линкер и флаги
BOOTLOADER = boot_files/link.ld
LDFLAGS = -T $(BOOTLOADER) -melf_i386

# Ассемблер и флаги
ASM = nasm
ASMFLAGS = -f elf32 -I./asm_files

# Папки
C_FILES = c_files
ASM_FILES = asm_files
OBJECT_FILES = object_files

# Исходные файлы
C_SRCS = $(wildcard $(C_FILES)/*.c)
ASM_SRCS = $(wildcard $(ASM_FILES)/*.s)
OBJECTS = $(patsubst $(C_FILES)/%.c, $(OBJECT_FILES)/%.o, $(C_SRCS)) \
          $(patsubst $(ASM_FILES)/%.s, $(OBJECT_FILES)/%.o, $(ASM_SRCS))

# Цели
all: $(OBJECTS) kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o $@

os.iso: kernel.elf
	mkdir -p iso/boot
	cp kernel.elf iso/boot/
	genisoimage -R \
                -b boot/grub/stage2_eltorito \
                -no-emul-boot \
                -boot-load-size 4 \
                -A os \
                -input-charset utf8 \
                -quiet \
                -boot-info-table \
                -o os.iso \
                iso

run: os.iso
	bochs -f configs/bochsrc.txt -q

# Правило для компиляции .c файлов
$(OBJECT_FILES)/%.o: $(C_FILES)/%.c
	$(CC) $(CFLAGS) $< -o $@

# Правило для ассемблирования .s файлов
$(OBJECT_FILES)/%.o: $(ASM_FILES)/%.s
	$(ASM) $(ASMFLAGS) $< -o $@

# Очистка
clean:
	rm -rf $(OBJECT_FILES)/*.o os.iso bochslog.txt kernel.elf com1.out
