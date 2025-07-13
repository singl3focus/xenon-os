#ifndef TB_H
#define TB_H

#include <stdint.h>

#define MAX_LINES 100      // Максимальное количество строк
#define LINE_SIZE 62      // Максимальная длина одной строки

typedef struct {
    char lines[MAX_LINES][LINE_SIZE]; // буфер текстовых строк
    int max_line_length;
    int current_line;
    int cursor_pos;                 // Позиция курсора в текущей строке
    uint32_t cursor_x;
    uint32_t cursor_y;                  
    int cursor_visible;             // Флаг видимости курсора
    int blink_state;                // Состояние мигания курсора
} TextBuffer;

void tb_init(TextBuffer *tb, int max_line_len);
void tb_clear(TextBuffer *tb);
void tb_insert_string(TextBuffer *tb, char c);
void tb_insert_char(TextBuffer *tb, char c);
void tb_delete_left(TextBuffer *tb);
void tb_move_cursor_left(TextBuffer *tb);
void tb_move_cursor_right(TextBuffer *tb);


#endif