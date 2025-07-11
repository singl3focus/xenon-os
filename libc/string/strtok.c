#include <string.h>

static char* save_ptr = NULL;

char* strtok(char* str, const char* delim) {
    char* token;
    if (str == NULL) {
        if (save_ptr == NULL) return NULL;
        str = save_ptr;
    }
    
    // Пропуск начальных разделителей
    str += strspn(str, delim);
    if (*str == '\0') {
        save_ptr = NULL;
        return NULL;
    }
    
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL) {
        save_ptr = NULL;
    } else {
        *str = '\0';
        save_ptr = str + 1;
    }
    return token;
}