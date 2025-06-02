#include <kernel/tty.h>
#include <kernel/drivers/serial.h>
#include <kernel/drivers/io.h>
#include <stdlib.h>
#include <string.h>

#include "../../arch/i386/gdt.h"
#include "../../arch/i386/idt.h"
#include "../../arch/i386/pic.h"
#include "../../arch/i386/ata.h"
#include "../../arch/i386/fat16.h"
#include "../../arch/i386/keyboard.h"
#include "../../arch/i386/fb.h"
#include "../../arch/i386/draw_logo.h"
#include "../../arch/i386/timer.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* OS will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

extern void parse_multiboot2(uint32_t magic, uint32_t addr);

void kernel_main(uint32_t magic, uint32_t addr) {
	// Инициализация последовательного порта
    serial_init();
    serial_puts("Serial port initialized\n");
	
	gdt_init();
    serial_puts("GDT initialized\n");

    // Вывод параметров Multiboot
    serial_puts("Multiboot magic: ");
	// if(magic != 0x36d76289) { ... }
    serial_put_hex(magic);
    serial_puts("\nMultiboot addr: ");
    serial_put_hex(addr);
    serial_puts("\n");

	// Перенастраиваем PIC на вектора 0x20-0x2F
	pic_remap(0x20, 0x28);
	serial_puts("PIC remapped\n");

	idt_init();
	serial_puts("IDT initialized\n");

	// Регистрируем IRQ обработчики
	timer_init();
	keyboard_init();
	serial_puts("Keyboard initialized\n");

	parse_multiboot2(magic, addr); // ← инициализация framebuffer
	serial_puts("Multiboot parsed\n");

    // Проверка фреймбуфера
    if (fb_info.address == 0) {
        serial_puts("ERROR: Framebuffer not initialized!\n");
        for (;;) asm volatile("hlt");
    }
    
    serial_puts("Framebuffer info:\n");
    serial_puts("  Address: "); serial_put_hex(fb_info.address);
    serial_puts("\n  Width: "); serial_put_dec(fb_info.width);
    serial_puts("\n  Height: "); serial_put_dec(fb_info.height);
    serial_puts("\n  Pitch: "); serial_put_dec(fb_info.pitch);
    serial_puts("\n  BPP: "); serial_put_dec(fb_info.bpp);
    serial_puts("\n");
    
	// Разрешаем прерывания
	asm volatile("sti");

    // Тест графики
    serial_puts("Testing graphics...\n");
    fb_clear(0x001F2126);
    //delay(1);

    
    // Основной логотип
    serial_puts("Drawing logo...\n");
    draw_logo();
    serial_puts("Logo drawn\n");

    fb_write("Welcome to Xenon OS!\n", 21);
    fb_write("Enter text:", 12);
    
	fb_cursor_blink_loop();

    // Чтение MBR
    uint8_t __attribute__((aligned(2))) mbr[512];
	check_drive(); // Перед чтением MBR
    ata_read_sectors(0, 1, mbr);
	serial_puts("ATA read sectors\n");


	for (;;) __asm__ volatile("hlt");
    // Проверка сигнатуры MBR
    if (mbr[510] == 0x55 && mbr[511] == 0xAA) {
        // Поиск раздела FAT16 в таблице разделов
        uint32_t fat_partition_lba = 0;
        int found = 0;
        
        for (int i = 0x1BE; i <= 0x1EE; i += 16) {
            if (mbr[i+4] == 0x04 || mbr[i+4] == 0x06 || mbr[i+4] == 0x0E) {
                fat_partition_lba = *(uint32_t*)(mbr + i + 8);
                found = 1;
                break;
            }
        }

        if (!found) {
			serial_puts("FAT16 partition not found in MBR\n");
            return;
        }
        
        char buf[32];
        itoa(fat_partition_lba, buf, 10);
        serial_puts("FAT16 partition found at LBA: ");
        serial_puts(buf);
        serial_puts("\n");
        
        fat16_init(fat_partition_lba);
    } else {
        // Проверка, является ли весь диск FAT16 разделом
        fat16_boot_sector_t bs;
        memcpy(&bs, mbr, sizeof(bs));
        
        // Проверка сигнатуры через прямой доступ к буферу
        if (mbr[510] == 0x55 && mbr[511] == 0xAA) {
            serial_puts("Full disk FAT16 partition\n");
            fat16_init(0);
        } else {
            serial_puts("Invalid disk signature\n");
            return;
        }
    }
    
    serial_puts("FAT16 initialized\n");
    serial_puts("Root directory:\n");
    fat16_list_root();
    
    for(;;) asm volatile("hlt");
}

void check_drive() {
    outb(ATA_DRIVE_SEL, 0xA0);
    if (inb(ATA_STATUS) == 0xFF) {
        serial_puts("No IDE drive detected!\n");
        return;
    }
    // TODO остальные проверки ...
}