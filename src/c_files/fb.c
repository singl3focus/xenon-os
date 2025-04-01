#include "io.h"

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

char *fb = (char *) 0x000B8000;

unsigned int cursor_x = 0;  // Текущая позиция по X (номер символа в строке)
unsigned int cursor_y = 0;  // Текущая позиция по Y (номер строки)

const unsigned int screen_width = 80;  // Ширина экрана в символах 
const unsigned int screen_height = 25; // Высота экрана

/*
    0x0	Черный	#000000
    0x1	Синий	#0000AA
    0x2	Зеленый	#00AA00
    0x3	Бирюзовый	#00AAAA
    0x4	Красный	#AA0000
    0x5	Фиолетовый	#AA00AA
    0x6	Коричневый	#AA5500
    0x7	Серый (светлый)	#AAAAAA
    0x8	Темно-серый	#555555
    0x9	Голубой	#5555FF
    0xA	Ярко-зеленый	#55FF55
    0xB	Бирюзовый (яркий)	#55FFFF
    0xC	Ярко-красный	#FF5555
    0xD	Розовый	#FF55FF
    0xE	Желтый	#FFFF55
    0xF	Белый	#FFFFFF
*/
const unsigned char fg = 0x0A;      
const unsigned char bg = 0x00;          

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(unsigned int i, char c, unsigned char bg, unsigned char fg) {
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_clear() {
    const char blank = ' '; // Пробел для очистки

    // Заполнение видеобуфера пробелами с атрибутами
    for (unsigned int y = 0; y < screen_height; y++) {
        for (unsigned int x = 0; x < screen_width; x++) {
            const unsigned int pos = y * screen_width + x;
            fb[pos * 2] = blank;          // Символ
            fb[pos * 2 + 1] = bg;  // Атрибуты цвета
        }
    }

    // Сброс позиции курсора
    cursor_x = 0;
    cursor_y = 0;
    fb_move_cursor(0);  // Перемещаем курсор в (0, 0)
}

/** fb_write:
 *  Writes the contents of the buffer buf of length len to the screen.
 *
 *  @param buf The buffer to write
 *  @param len The length of the buffer
 */
int fb_write(char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        char c = buf[i];

         // Проверка на выход за границы экрана
         if (cursor_y >= screen_height) {
            // todo: прокрутка экрана 
            cursor_y = screen_height - 1;
        }

        if (c == '\n') {  // Обработка переноса строки
            cursor_x = 0;          
            cursor_y++;            
        } else {
            unsigned int pos = cursor_y * screen_width + cursor_x;
            fb_write_cell(pos * 2, c, fg, bg);  
            
            cursor_x++;
        }

        if (cursor_x >= screen_width) {  // Если достигли конца строки
            cursor_x = 0;
            cursor_y++;
        }

        unsigned int cursor_pos = cursor_y * screen_width + cursor_x;
        fb_move_cursor(cursor_pos);           
    }

    return len;
}
