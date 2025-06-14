#include "fb.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "types.h"
#include "bga.h"
#include "draw_logo.h"

extern Framebuffer_Info fb_info;

void kmain() {
    // serial_init(); // not implemented
    
    init_idt();
    pic_remap();
    
    keyboard_init();
    
    // Инициализация BGA
    bga_init();

    // Получение адреса фреймбуфера через PCI BAR0 (для Bochs)
    fb_info.address = 0xE0000000; // Стандартный адрес LFB для Bochs
    fb_info.width = 1024;
    fb_info.height = 768;
    fb_info.pitch = fb_info.width * 4; // 32bpp = 4 байта на пиксель
    fb_info.bpp = 32;

    fb_clear(0x001F2126); // Очистка экрана
    draw_logo();

    fb_write("Welcome to Xenon OS!\n", 21);
    fb_write("Enter text:", 12);

    asm volatile("sti"); // Включить прерывания

    fb_cursor_blink_loop(15000000);
    
    while(1);
}
