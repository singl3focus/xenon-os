#include <kernel/tty.h>

#include "../../arch/i386/gdt.h"
#include "../../arch/i386/idt.h"
#include "../../arch/i386/pic.h"
#include "../../arch/i386/keyboard.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This kernel needs to be compiled with a ix86-elf compiler"
#endif

void kernel_main(void) {
	terminal_initialize();

	// Перенастраиваем PIC на вектора 0x20-0x2F
	pic_remap(0x20, 0x28);

	idt_init();

	// Регистрируем IRQ обработчики
	keyboard_init();

	// Разрешаем прерывания
	asm volatile("sti");
	
	terminal_writestring("Hello, kernel World!\n");

	asm volatile("int $0x00");

	for (;;) __asm__ volatile("hlt");
}