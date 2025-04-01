#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "fb.h"
#include "types.h"

#define KBD_DATA 0x60

void keyboard_init(void) {    
    // Разрешаем IRQ1 в PIC
    pic_clear_mask(1);
}

// Таблица преобразования скан-кодов (для клавиш без Shift)
static const char kbd_us[] = {
    0, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

void keyboard_handler(void) {
    uint8_t scancode = inb(KBD_DATA); // Читаем скан-код
    
    // Игнорируем отпускание клавиши (старший бит 1)
    if (!(scancode & 0x80)) {
        char c = kbd_us[scancode];
        if (c != 0) {
            fb_write(&c, 1); // Выводим символ на экран
        }
    }
    
    // (уже делается в irq_handler_c)
    // // Отправляем EOI в оба контроллера PIC 
    // outb(0x20, 0x20);
    // outb(0xA0, 0x20);
}
