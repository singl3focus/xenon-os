#include <string.h>

char* strncat(char* dest, const char* src, size_t n) {
    char* d = dest;

    // Перемещаемся в конец строки dest
    while (*d) d++;

    // Копируем не более n символов
    while (n-- && *src) {
        *d++ = *src++;
    }

    *d = '\0';
    return dest;
}
