#include <kernel/tty.h>
#include <kernel/drivers/serial.h>

#include "../../arch/i386/gdt.h"
#include "../../arch/i386/idt.h"
#include "../../arch/i386/pic.h"
#include "../../arch/i386/keyboard.h"
#include "../../arch/i386/fb.h"
#include "../../arch/i386/draw_logo.h"
#include "../../arch/i386/timer.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
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
	timer_init(2000);
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

	asm volatile("int $0x00");

	fb_cursor_blink_loop();

	for (;;) __asm__ volatile("hlt");
}