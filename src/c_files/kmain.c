#include "fb.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"
#include "types.h"

extern uint8_t vbe_mode_info[256]; // Внешняя ссылка на данные VBE

void kmain() {
    fb_clear(); // * it's provide some not default work now, but in the future - we will fix it
    // serial_init(); // not implemented
    
    init_idt();
    pic_remap();
    
    keyboard_init();
    
    asm volatile("sti"); // Включить прерывания
    
    // Извлечение информации из VBE Mode Info Block
    fb_info.address = (uintptr_t)(*(uint32_t*)(vbe_mode_info + 0x28)); // PhysBasePtr
    fb_info.width = *(uint16_t*)(vbe_mode_info + 0x12);   // XResolution
    fb_info.height = *(uint16_t*)(vbe_mode_info + 0x14);  // YResolution
    fb_info.pitch = *(uint16_t*)(vbe_mode_info + 0x10);   // BytesPerScanline
    fb_info.bpp = *(uint8_t*)(vbe_mode_info + 0x1B);      // BitsPerPixel

    // Пример: заполнение экрана красным цветом
    uint32_t color = 0xFF0000; // RGB (24/32bpp)
    for (uint32_t y = 0; y < fb_info.height; y++) {
        for (uint32_t x = 0; x < fb_info.width; x++) {
            uint32_t* pixel = (uint32_t*)((uint8_t*)fb_info.address + y * fb_info.pitch + x * 4);
            *pixel = color;
        }
    }

    // Очистка экрана
    draw_rect(0, 0, fb_info.width, fb_info.height, 0xFF0000);

    draw_rect(100, 100, 200, 100, 0x00FF00);

    while(1);
}
