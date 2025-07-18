#ifndef FB_H
#define FB_H

#include <stdint.h>

typedef struct {
    uint32_t address;   // Адрес framebuffer
    uint32_t pitch;     // Количество байт на строку
    uint32_t width;     // Ширина экрана (в пикселях)
    uint32_t height;    // Высота экрана (в пикселях)
    uint8_t  bpp;       // Бит на пиксель
} Framebuffer_Info;

extern Framebuffer_Info fb_info;

void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_line(uint32_t x, uint32_t y, uint32_t width, uint32_t color);
void draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color, int scale);
void draw_string(int x, int y, const char* str, uint32_t fg_color, uint32_t bg_color, int scale);

void fb_clear(uint32_t color);
void fb_reset_state(void);
int fb_write(const char *buf);
void fb_handle_cursor_blink(void);
void fb_backspace(void);
void fb_insert_char(char c, int was_at_end);
void fb_move_cursor_right(void);
void fb_move_cursor_left(void);
void fb_move_cursor_up(void);
void fb_move_cursor_down(void);

void fb_init(uint64_t address, uint32_t pitch, uint32_t width, uint32_t height, uint8_t bpp);

#endif