#include <kernel/drivers/io.h>
#include <stdint.h>
#include "fb.h"
#include "keyboard.h"
#include "pic.h"

#define KBD_DATA 0x60

void keyboard_init(void) {    
    // Разрешаем IRQ1 в PIC
    pic_send_eoi(1);
}

// Таблица преобразования скан-кодов (для клавиш без Shift)
static const char kbd_us[] = {
    0, 0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x08,
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 0, 0, ' '
};

// Таблица преобразования скан-кодов (для клавиш c Shift)
static const char kbd_us_shift[] = {
    0, 0x1B, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x08,
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 0, 0, ' '
};

// Глобальные состояния
static int shift_pressed = 0;
static int caps_lock_enabled = 0;

void keyboard_handler(void) {
    uint8_t scancode = inb(KBD_DATA);

    // Проверяем отпускание (release)
    int key_released = scancode & 0x80;
    uint8_t code = scancode & 0x7F;

    if (key_released) {
        // Отслеживание отпускания Shift
        if (code == 0x2A || code == 0x36) {
            shift_pressed = 0;
        }
    } else {
        // Отслеживание нажатия Shift
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = 1;
            return;
        }

        // Отслеживание нажатия Caps Lock (переключение)
        if (scancode == 0x3A) {
            caps_lock_enabled = !caps_lock_enabled;
            return;
        }

        char c = 0;
        if (scancode < sizeof(kbd_us)) {
            // Выбор таблицы: shift или нет
            int use_shift = shift_pressed;
            char base_char = kbd_us[scancode];
            char shift_char = kbd_us_shift[scancode];

            if (base_char >= 'a' && base_char <= 'z') {
                // Буквы: учитывать CapsLock XOR Shift
                if (caps_lock_enabled ^ shift_pressed) {
                    c = shift_char;
                } else {
                    c = base_char;
                }
            } else {
                // Не буквы: учитывать только Shift
                c = use_shift ? shift_char : base_char;
            }

            if (c != 0) {
                fb_write(&c, 1); // Выводим символ
            }
        }
    }
}

int keyboard_is_shift_pressed(void) {
    return shift_pressed;
}

int keyboard_is_capslock_enabled(void) {
    return caps_lock_enabled;
}