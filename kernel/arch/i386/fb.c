#include "fb.h"
#include "font.h"
#include "timer.h"
#include <kernel/drivers/io.h>
#include <string.h>

Framebuffer_Info fb_info;

#define MAX_LINE_CHARS 128
static char current_line[MAX_LINE_CHARS] = {0};
static int current_line_len = 0;
static int current_line_pos = 0;

static int cursor_visible = 0;
static const int CURSOR_WIDTH = 1;
static const uint32_t CURSOR_COLOR = 0xFFFFFF;
static uint32_t cursor_x = 15;
static uint32_t cursor_y = 120;
static const int CHAR_WIDTH = 8;
static const int CHAR_HEIGHT = 8;
static const int SCALE = 2;
static const uint32_t FG_COLOR = 0xFFFFFF;
static const uint32_t BG_COLOR = 0x1F2126;
static const uint32_t LINE_HEIGHT = CHAR_HEIGHT * SCALE + 16;

static uint32_t last_cursor_blink = 0;

// Объявление вспомогательных функций
static void redraw_current_line(void);
static void fb_new_line(void);
static void fb_wrap_line(int was_at_end);

void fb_init(uint64_t address, uint32_t pitch, uint32_t width, uint32_t height, uint8_t bpp) {
    fb_info.address = (uint32_t)address;
    fb_info.pitch = pitch;
    fb_info.width = width;
    fb_info.height = height;
    fb_info.bpp = bpp;
    
    // Инициализируем начальное состояние
    memset(current_line, 0, sizeof(current_line));
    current_line_len = 0;
    current_line_pos = 0;
    cursor_x = 15;
    cursor_y = 120;
    cursor_visible = 1;  // Курсор видим изначально
    
    // Отрисовка начального курсора
    draw_rect(cursor_x, cursor_y, CURSOR_WIDTH, CHAR_HEIGHT * SCALE, CURSOR_COLOR);
}

void draw_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_info.width || y >= fb_info.height || fb_info.address == 0) return;
    uint32_t *pixel = (uint32_t *)(fb_info.address + y * fb_info.pitch + x * 4);
    *pixel = color;
}

void draw_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t dy = 0; dy < height; dy++) {
        for (uint32_t dx = 0; dx < width; dx++) {
            draw_pixel(x + dx, y + dy, color);
        }
    }
}

void draw_char(int x, int y, char c, uint32_t fg_color, uint32_t bg_color, int scale) {
    for (int row = 0; row < CHAR_HEIGHT; row++) {
        uint8_t bits = font8x8_basic[(unsigned char)c][row];
        for (int col = 0; col < CHAR_WIDTH; col++) {
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
    int cx = x;
    while (*str) {
        draw_char(cx, y, *str, fg_color, bg_color, scale);
        cx += CHAR_WIDTH * scale;
        str++;
    }
}

void fb_clear(uint32_t color) {
    draw_rect(0, 0, fb_info.width, fb_info.height, color);
}

void fb_reset_state() {
    memset(current_line, 0, sizeof(current_line));
    current_line_len = 0;
    current_line_pos = 0;
    cursor_x = 15;
    cursor_y = 20;
    cursor_visible = 1;
}


void fb_scroll(void) {
    uint32_t shift_bytes = LINE_HEIGHT * fb_info.pitch;
    uint32_t screen_bytes = fb_info.pitch * fb_info.height;

    uint8_t* fb_ptr = (uint8_t*)fb_info.address;
    for (uint32_t i = 0; i + shift_bytes < screen_bytes; i++) {
        fb_ptr[i] = fb_ptr[i + shift_bytes];
    }
    
    uint32_t clear_start = fb_info.height - LINE_HEIGHT;
    for (uint32_t y = clear_start; y < fb_info.height; y++) {
        for (uint32_t x = 0; x < fb_info.width; x++) {
            draw_pixel(x, y, BG_COLOR);
        }
    }
    
    // Корректируем позицию курсора после скролла
    if (cursor_y >= LINE_HEIGHT) {
        cursor_y -= LINE_HEIGHT;
    } else {
        cursor_y = 0;
    }
}

void show_cursor() {
    cursor_visible = 1;
    draw_rect(cursor_x, cursor_y, CURSOR_WIDTH, CHAR_HEIGHT * SCALE, CURSOR_COLOR);
}

void hide_cursor() {
    cursor_visible = 0;
    draw_rect(cursor_x, cursor_y, CURSOR_WIDTH, CHAR_HEIGHT * SCALE, BG_COLOR);
}

void fb_toggle_cursor(void) {
    if (cursor_visible) {
        hide_cursor();
    } else {
        show_cursor();
    }
}

void fb_handle_cursor_blink() {
    uint32_t current_ticks = get_timer_ticks();
    
    if (current_ticks - last_cursor_blink >= 1000) { // 1000ms = 1s 
        fb_toggle_cursor();
        last_cursor_blink = current_ticks;
    }
}


int fb_write(const char *buf) {
    unsigned int len = strlen(buf);
    for (unsigned int i = 0; i < len; ++i) {
        char c = buf[i];
        hide_cursor();  // Всегда скрываем курсор перед выводом

        if (c == '\n') {
            fb_new_line();
            continue;
        }

        // Запоминаем, был ли курсор в конце до вставки
        int was_at_end = (current_line_pos == current_line_len);
        fb_insert_char(c, was_at_end);

        if (!cursor_visible) fb_toggle_cursor();
    }
    return (int)len;
}

void fb_backspace(void) {
    if (cursor_visible) fb_toggle_cursor();

    if (current_line_pos == 0 || current_line_len == 0) {
        if (cursor_visible) fb_toggle_cursor();
        return;
    }

    // Удаляем символ слева от курсора
    for (int i = current_line_pos - 1; i < current_line_len - 1; i++) {
        current_line[i] = current_line[i + 1];
    }
    current_line_len--;
    current_line[current_line_len] = '\0';
    current_line_pos--;

    redraw_current_line();

    if (!cursor_visible) fb_toggle_cursor();
}

void fb_move_cursor_left(void) {
    if (cursor_visible) fb_toggle_cursor();
    if (current_line_pos > 0) {
        current_line_pos--;
        cursor_x -= CHAR_WIDTH * SCALE;
    }
    if (!cursor_visible) fb_toggle_cursor();
}

void fb_move_cursor_right(void) {
    if (cursor_visible) fb_toggle_cursor();
    if (current_line_pos < current_line_len) {
        current_line_pos++;
        cursor_x += CHAR_WIDTH * SCALE;
    }
    if (!cursor_visible) fb_toggle_cursor();
}

void fb_move_cursor_up(void) {
    // Можно реализовать переход к предыдущей «логической» строке
}
void fb_move_cursor_down(void) {
    // Можно реализовать переход к следующей «логической» строке
}

void fb_insert_char(char c, int was_at_end) {
    if (cursor_visible) fb_toggle_cursor();

    if (current_line_len >= MAX_LINE_CHARS - 1) {
        if (!cursor_visible) fb_toggle_cursor();
        return;
    }

    for (int i = current_line_len; i > current_line_pos; i--) {
        current_line[i] = current_line[i - 1];
    }
    current_line[current_line_pos] = c;
    current_line_len++;
    current_line_pos++;
    current_line[current_line_len] = '\0';

    // Всегда проверяем необходимость переноса
    uint32_t string_width = current_line_len * CHAR_WIDTH * SCALE;
    if (15 + string_width > fb_info.width - 15) {
        fb_wrap_line(was_at_end);
    } else {
        redraw_current_line();
    }

    if (cursor_y + CHAR_HEIGHT * SCALE > fb_info.height) {
        fb_scroll();
    }

    if (!cursor_visible) fb_toggle_cursor();
}

// ------------------------------------------------------------------
// Ниже идут вспомогательные функции, не объявленные в fb.h — оставляем их static.

// Перерисовать текущую строку целиком (очистить фон → вывести текст)
static void redraw_current_line(void) {
    // Заливаем область текущей строки фоном:
    draw_rect(15, cursor_y, fb_info.width - 30, CHAR_HEIGHT * SCALE, BG_COLOR);
    // Отрисовываем текст
    draw_string(15, cursor_y, current_line, FG_COLOR, BG_COLOR, SCALE);
    // Обновляем положение курсора
    cursor_x = 15 + current_line_pos * CHAR_WIDTH * SCALE;

    if (cursor_visible) {
        draw_rect(cursor_x, cursor_y, CURSOR_WIDTH, CHAR_HEIGHT * SCALE, CURSOR_COLOR);
    }
}

// Перенести на следующую строку (сохранить cursor_x = 15, увеличить cursor_y и, при необходимости, прокрутить)
static void fb_new_line(void) {
    cursor_x = 15;
    cursor_y += LINE_HEIGHT;
    if (cursor_y + CHAR_HEIGHT * SCALE > fb_info.height) {
        fb_scroll();
    }
    
    // Сбрасываем состояние для новой строки
    memset(current_line, 0, sizeof(current_line));
    current_line_len = 0;
    current_line_pos = 0;
}

// Если текущая строка переполнилась по ширине, 
// «отрезаем» символы справа и переносим их в новую строку
static void fb_wrap_line(int was_at_end) {
    int max_chars_per_line = (fb_info.width - 30) / (CHAR_WIDTH * SCALE);
    if (max_chars_per_line <= 0) return;
    
    // Проверяем необходимость переноса для текущей строки
    while (current_line_len > max_chars_per_line) {
        // Определяем точку переноса (последний пробел или max_chars)
        int wrap_pos = max_chars_per_line;
        for (int i = wrap_pos; i > 0; i--) {
            if (current_line[i] == ' ') {
                wrap_pos = i;
                break;
            }
        }

        // Выделяем часть для переноса
        char new_line[MAX_LINE_CHARS] = {0};
        int new_line_len = current_line_len - wrap_pos - 1;
        memcpy(new_line, current_line + wrap_pos + 1, new_line_len);
        new_line[new_line_len] = '\0';
        
        // Обрезаем текущую строку
        current_line[wrap_pos] = '\0';
        current_line_len = wrap_pos;
        
        // Обновляем позицию курсора
        if (current_line_pos > wrap_pos) {
            current_line_pos -= wrap_pos + 1;
        } else if (current_line_pos == current_line_len && was_at_end) {
            current_line_pos = new_line_len;
        }
        
        // Перерисовываем текущую строку
        redraw_current_line();
        
        // Переходим на новую строку
        fb_new_line();
        
        // Копируем перенесённую часть в новую строку
        memcpy(current_line, new_line, new_line_len);
        current_line_len = new_line_len;
        current_line[current_line_len] = '\0';
        
        // Регулируем позицию курсора
        if (was_at_end && current_line_pos != new_line_len) {
            current_line_pos = new_line_len;
        }
        
        // Проверяем необходимость переноса для новой строки
        redraw_current_line();
    }
}