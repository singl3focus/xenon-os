#include "fb.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "types.h"
#include "bga.h"

extern uint8_t vbe_mode_info[256]; // Внешняя ссылка на данные VBE
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

    // Пример рисования
    draw_rect(0, 0, fb_info.width, fb_info.height, 0x000000); // Очистка экрана
    draw_rect(100, 100, 200, 150, 0xFF0000); // Красный прямоугольник

    asm volatile("sti"); // Включить прерывания

    while(1);
}
