#ifndef FB_H
#define FB_H

#include "types.h"

extern Framebuffer_Info fb_info;

void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_line(uint32_t x, uint32_t y, uint32_t width, uint32_t color);
void draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color, int scale);
void draw_string(int x, int y, const char* str, uint32_t fg_color, uint32_t bg_color, int scale);
void delay(unsigned int count);

void fb_clear();
int fb_write(char *buf, unsigned int len);

#endif
