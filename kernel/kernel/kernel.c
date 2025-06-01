#include <kernel/tty.h>
#include <kernel/drivers/io.h>
#include <kernel/drivers/serial.h>

#include "../../arch/i386/gdt.h"
#include "../../arch/i386/idt.h"
#include "../../arch/i386/pic.h"
#include "../../arch/i386/paging.h"
#include "../../arch/i386/keyboard.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* OS only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(uint32_t magic, uint32_t addr) {
	serial_init();
    serial_puts("Serial port initialized\n");
    
    // Вывод параметров Multiboot
    serial_puts("Multiboot magic: ");
    serial_put_hex(magic);
    serial_puts("\nMultiboot addr: ");
    serial_put_hex(addr);
    serial_puts("\n");

	terminal_initialize();

	// Инициализация прерываний
	pic_remap(0x20, 0x28); // Перенастраиваем PIC на вектора 0x20-0x2F
	serial_puts("PIC remapped\n");

	idt_init();
	serial_puts("IDT initialized\n");
	
	keyboard_init(); // Регистрируем обработчик
	asm volatile("sti"); // Разрешаем прерывания
	serial_puts("Interrupts allowed\n");
	
	// Инициализация постоянного пейджинга
    // paging_init();
	
	terminal_writestring("Hello, kernel World!\n");
	serial_puts("Hello printed\n");
	
	// Тест прерывания
	asm volatile("int $0x00");
	serial_puts("Int 0x00 test\n");

	// // Тест доступа к виртуальному адресу
    // int *ptr = (int*)0x100000; // 1MB виртуальный адрес
    // *ptr = 42;
    // terminal_writestring("Memory write at 1MB successful\n");
    
    // // Тест доступа к адресу в области ядра
    // int *kptr = (int*)0xC0000000;
    // terminal_writestring("Kernel space test... ");
    // *kptr = 42; // Должно работать, если paging_init настроил mapping
    // terminal_writestring("OK\n");

	/*
	0x100000 (1MB) - работает через identity mapping
	0xC0000000 - работает через mapping ядра
	*/

	for (;;) __asm__ volatile("hlt");
}