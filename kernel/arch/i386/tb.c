#include "tb.h"
#include <string.h>
#include <stdarg.h>

void tb_init(TextBuffer *tb, int max_line_len) {
    memset(tb, 0, sizeof(TextBuffer));
    tb->cursor_visible = 1;
    tb->blink_state = 1;
    tb->max_line_length = max_line_len;
    tb->cursor_pos = 0;
    strcpy(tb->lines[0], "");
}

void tb_clear(TextBuffer *tb) {
    // Сбрасываем состояние буфера
    tb->current_line = 0;
    tb->cursor_pos = 0;
    
    // Очищаем все строки
    for (int i = 0; i < MAX_LINES; i++) {
        tb->lines[i][0] = '\0';
    }
    
    // Инициализируем первую строку как пустую
    strcpy(tb->lines[0], "");
}

static void tb_shift_text(TextBuffer *tb){
    int line = tb->current_line;
    int pos = tb->cursor_pos;
    int len = strlen(tb->lines[line]);

    for (int i = len; i >= pos; i--) {
        tb->lines[line][i + 1] = tb->lines[line][i];
    }
}

void tb_insert_string(TextBuffer *tb, char c) {
    int line = tb->current_line;
    int pos = tb->cursor_pos;
    int len = strlen(tb->lines[line]);
    int max_len = tb->max_line_length;

    // Если есть место в текущей строке
    if (len < max_len) {
        // Сдвиг символов вправо
        tb_shift_text(tb);
        tb->lines[line][pos] = c;
        tb->cursor_pos++;
        tb->lines[line][len + 1] = '\0'; // Обновляем терминатор
    } 
    // Если строка полная и достигнут предел строк
    else if (line + 1 >= MAX_LINES) {
        return; // Невозможно вставить
    }
    // Перенос на новую строку
    else {
        int tail_len = len - pos; // Длина "хвоста" для переноса

        // Если курсор не в конце строки
        if (tail_len > 0) {
            // Переносим "хвост" на новую строку
            strcpy(tb->lines[line + 1], &tb->lines[line][pos]);
            tb->lines[line][pos] = '\0'; // Обрезаем текущую строку
        }

        // Вставляем символ
        if (tail_len > 0) {
            // В конец текущей строки
            tb->lines[line][pos] = c;
            tb->lines[line][pos + 1] = '\0';
            tb->cursor_pos = pos + 1;
        } else {
            // В начало новой строки
            tb->lines[line + 1][0] = c;
            tb->lines[line + 1][1] = '\0';
            tb->current_line++;
            tb->cursor_pos = 1;
        }
    }
}

void tb_insert_char(TextBuffer *tb, char c) {
    int line = tb->current_line;
    int pos = tb->cursor_pos;
    int len = strlen(tb->lines[line]);
    int max_len = tb->max_line_length;
    int was_at_end = (pos == len);  // Флаг: курсор был в конце строки

    // Если есть место в текущей строке (макс. длина max_len)
    if (len < max_len) {
        // Сдвиг символов
        for (int i = len; i >= pos; i--) {
            tb->lines[line][i + 1] = tb->lines[line][i];
        }
        tb->lines[line][pos] = c;
        tb->lines[line][max_len] = '\0';
        tb->cursor_pos++;
    }
    else if (line + 1 >= MAX_LINES) {
        return;
    }
    // Перенос последнего символа
    else {
        // Если вставка была в конец - перемещаем курсор на новую строку
        if (was_at_end) {
            tb->current_line++;

            int next_line = tb->current_line;
            int next_len = strlen(tb->lines[next_line]);

            for (int i = next_len; i >= pos; i--) {
                tb->lines[next_line][i + 1] = tb->lines[next_line][i];
            }
            // Вставка символа в новую строку
            tb->lines[line][max_len - 1] = '\0';
            tb->lines[next_line][0] = c;
            tb->lines[next_line][max_len - 1] = '\0'; // Важно: установка терминатора
            tb->cursor_pos = 1; // Курсор после символа
        } else {
            // Сохраняем последний символ перед сдвигом
            char overflow_char = tb->lines[line][max_len - 1];
            
            // Сдвигаем только до max_line_length - 1
            for (int i = max_len - 2; i >= pos; i--) {
                tb->lines[line][i + 1] = tb->lines[line][i];
            }
            tb->lines[line][pos] = c;
            tb->cursor_pos++;

            // Вставка вытесненного символа в новую строку
            int next_line = line + 1;
            int next_len = strlen(tb->lines[next_line]);
            
            // Сдвиг символов новой строки
            for (int i = next_len; i >= 0; i--) {
                tb->lines[next_line][i + 1] = tb->lines[next_line][i];
            }
            tb->lines[line][max_len] = '\0';
            tb->lines[next_line][0] = overflow_char;
            tb->lines[next_line][max_len - 1] = '\0';
            
        }
    }
}

void tb_delete_left(TextBuffer *tb) {
    int line = tb->current_line;
    int pos = tb->cursor_pos;
    int len = strlen(tb->lines[line]);
    
    if (pos > 0) {
        // Сдвигаем символы влево
        for (int i = pos - 1; i < len; i++) {
            tb->lines[line][i] = tb->lines[line][i + 1];
        }
        tb->cursor_pos--;
    } else if (line > 0) {
        // Объединение с предыдущей строкой
        int prev_len = strlen(tb->lines[line - 1]);
        int curr_len = strlen(tb->lines[line]);
        
        if (prev_len + curr_len < LINE_SIZE - 1) {
            // Копируем текущую строку в конец предыдущей
            strcat(tb->lines[line - 1], tb->lines[line]);
            
            // Удаляем текущую строку
            for (int i = line; i < line; i++) {
                strcpy(tb->lines[i], tb->lines[i + 1]);
            }
            tb->current_line--;
            tb->cursor_pos = prev_len;
        }
    }
}

void tb_move_cursor_left(TextBuffer *tb) {
    if (tb->cursor_pos > 0) {
        tb->cursor_pos--;
    } else if (tb->current_line > 0) {
        tb->current_line--;
        tb->cursor_pos = strlen(tb->lines[tb->current_line]);
    }
}

void tb_move_cursor_right(TextBuffer *tb) {
    int line = tb->current_line;
    int len = strlen(tb->lines[line]);
    
    // if (tb->cursor_pos < len) {
    //     tb->cursor_pos++;
    // } else if (line < tb->line_count - 1) {
    //     tb->current_line++;
    //     tb->cursor_pos = 0;
    // }
} 