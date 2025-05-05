#include "fb.h"
#include "io.h"
#include "font.h"

#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

#define MAX_COLS     1024 /* количество столбцов экрана               */
#define MAX_ROWS     768 /* количество строк экрана                  */

char *fb = (char *) 0x000B8000;

unsigned int cursor_x = 0;  // Текущая позиция по X (номер символа в строке)
unsigned int cursor_y = 0;  // Текущая позиция по Y (номер строки)

const unsigned int screen_width = 80;  // Ширина экрана в символах 
const unsigned int screen_height = 25; // Высота экрана

const unsigned char fg = 0x0A;      
const unsigned char bg = 0x00;  

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_info.width || y >= fb_info.height) return;
    uint32_t* pixel = (uint32_t*)((uint8_t*)fb_info.address + y * fb_info.pitch + x * 4);
    *pixel = color; // Формат 0x00RRGGBB
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t i = 0; i < height; i++) {
        for (uint32_t j = 0; j < width; j++) {
            draw_pixel(x + j, y + i, color);
        }
    }
}

void draw_line(uint32_t x, uint32_t y, uint32_t width, uint32_t color){
    for (uint32_t i = x; i < x + width; i++) {
        draw_pixel(i, y, color);
    } 
}

void draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color, int scale) {
    for (int row = 0; row < 8; row++) {
        uint8_t bits = font8x8_basic[(int)c][row];
        for (int col = 0; col < 8; col++) {
            uint32_t color = (bits & (1 << col)) ? fg_color : bg_color;
            for (int dy = 0; dy < scale; dy++) {
                for (int dx = 0; dx < scale; dx++) {
                    draw_pixel(x + col * scale + dx, y + row * scale + dy, color);
                }
            }
        }
    }
}

void draw_string(int x, int y, const char* str, uint32_t fg_color, uint32_t bg_color, int scale) {
    int cursor_x = x;
    while (*str) {
        draw_char(cursor_x, y, *str, fg_color, bg_color, scale);
        cursor_x += 8*scale;  // фиксированная ширина символа
        str++;
    }
}

void delay(unsigned int count) {
    for (volatile unsigned int i = 0; i < count; i++) {
        // Простой пустой цикл — задержка
        __asm__ __volatile__("nop");
    }
}

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
            // TODO: прокрутка экрана 
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