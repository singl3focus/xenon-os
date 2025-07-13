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

// Специальные скан-коды (второй байт после 0xE0)
#define SC_LEFT  0x4B
#define SC_RIGHT 0x4D
#define SC_UP    0x48
#define SC_DOWN  0x50

static int e0_code = 0;  // Флаг для префикса 0xE0

void keyboard_handler(void) {
    uint8_t scancode = inb(KBD_DATA);

    if (scancode == 0xE0) {
        e0_code = 1;
        return;
    }

    if (e0_code) {
        // Обработка стрелок и других E0-команд
        switch (scancode) {
            case SC_LEFT:
                fb_move_cursor_left();
                break;
            case SC_RIGHT:
                fb_move_cursor_right();
                break;
            case SC_UP:
                fb_move_cursor_up();
                break;
            case SC_DOWN:
                fb_move_cursor_down();
                break;
        }
        e0_code = 0;
        return;
    }

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
            char base = kbd_us[scancode];
            char shifted = kbd_us_shift[scancode];

            if (base >= 'a' && base <= 'z') {
                c = (caps_lock_enabled ^ shift_pressed) ? shifted : base;
            } else {
                c = shift_pressed ? shifted : base;
            }

            // Нажат Backspace
            if (scancode == 0x0E) {
                shell_handle_input('\b');
                return;
            }

            if (c) {
                shell_handle_input(c);
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

