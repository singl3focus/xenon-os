#include "fb.h"
#include "io.h"
#include "font.h"

Framebuffer_Info fb_info;

static uint32_t cursor_x = 15;
static uint32_t cursor_y = 120;
static const int CHAR_WIDTH = 8;
static const int CHAR_HEIGHT = 8;
static const int SCALE = 2;  // Масштаб шрифта (можно поменять)
static const uint32_t FG_COLOR = 0xFFFFFF;  // Белый
static const uint32_t BG_COLOR = 0x1F2126;  // Чёрный

void fb_init(uint64_t address, uint32_t pitch, uint32_t width, uint32_t height, uint8_t bpp) {
    fb_info.address = address;
    fb_info.pitch = pitch;
    fb_info.width = width;
    fb_info.height = height;
    fb_info.bpp = bpp;
}

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

void fb_clear(uint32_t color) {
    draw_rect(0, 0, fb_info.width, fb_info.height, color);
}

void delay(unsigned int count) {
    for (volatile unsigned int i = 0; i < count; i++) {
        // Простой пустой цикл — задержка
        __asm__ __volatile__("nop");
    }
}

void fb_scroll() {
    uint32_t row_height = CHAR_HEIGHT * SCALE;
    uint32_t screen_bytes = fb_info.pitch * fb_info.height;
    uint8_t *fb_ptr = (uint8_t*) fb_info.address;

    // Количество байт, соответствующее сдвигу на 1 строку
    uint32_t shift_bytes = row_height * fb_info.pitch;

    // Сдвигаем framebuffer вверх на одну строку
    for (uint32_t i = 0; i < screen_bytes - shift_bytes; i++) {
        fb_ptr[i] = fb_ptr[i + shift_bytes];
    }

    // Очищаем последнюю строку
    uint32_t clear_y_start = fb_info.height - row_height;
    for (uint32_t y = clear_y_start; y < fb_info.height; y++) {
        for (uint32_t x = 0; x < fb_info.width; x++) {
            draw_pixel(x, y, BG_COLOR);
        }
    }

    // Перенос курсора на начало последней строки
    cursor_x = 15;
    cursor_y = clear_y_start;
}

int fb_write(const char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        char c = buf[i];

        if (c == '\n') {
            cursor_x = 15;
            cursor_y += CHAR_HEIGHT * SCALE + 16;
            if (cursor_y + CHAR_HEIGHT * SCALE > fb_info.height) {
                fb_scroll();
            }
            continue;
        }

        draw_char(cursor_x, cursor_y, c, FG_COLOR, BG_COLOR, SCALE);
        cursor_x += CHAR_WIDTH * SCALE;

        if (cursor_x + CHAR_WIDTH * SCALE > fb_info.width - 10) {
            cursor_x = 15;
            cursor_y += CHAR_HEIGHT * SCALE + 16;
            if (cursor_y + CHAR_HEIGHT * SCALE > fb_info.height) {
                fb_scroll();
            }
        }
    }

    return len;
}
