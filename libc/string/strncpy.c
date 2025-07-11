#include <string.h>

char* strncpy(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (n-- && (*dest++ = *src++));
    while (n--) *dest++ = '\0';
    return ret;
}