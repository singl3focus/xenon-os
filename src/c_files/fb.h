#ifndef FB_H
#define FB_H

#include "types.h"

extern Framebuffer_Info fb_info;

void draw_pixel(uint32_t x, uint32_t y, uint32_t color);
void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);

void fb_clear();
int fb_write(char *buf, unsigned int len);

#endif
