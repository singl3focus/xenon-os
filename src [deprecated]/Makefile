# Компилятор и флаги
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c \
         -Iinclude

LD = ld
LDFLAGS = -T boot_files/link.ld -melf_i386

ASM = nasm
ASMFLAGS = -f elf32

# Папки
C_FILES = c_files
ASM_FILES = asm_files
OBJECT_FILES = object_files

C_SRCS = $(wildcard $(C_FILES)/*.c)
ASM_SRCS = $(wildcard $(ASM_FILES)/*.s)
OBJECTS = $(patsubst $(C_FILES)/%.c, $(OBJECT_FILES)/%.o, $(C_SRCS)) \
          $(patsubst $(ASM_FILES)/%.s, $(OBJECT_FILES)/%.o, $(ASM_SRCS))

all: $(OBJECTS) kernel.elf

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJECT_FILES)/%.o: $(C_FILES)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJECT_FILES)/%.o: $(ASM_FILES)/%.s
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(OBJECT_FILES)/*.o os.iso bochslog.txt kernel.elf com1.out

os.iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/
	cp configs/grub.cfg iso/boot/grub/
	grub-mkrescue -o os.iso iso

run: os.iso
	bochs -f configs/bochsrc.txt -q
