#include <string.h>

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        const char* a = accept;
        while (*a) {
            if (*s == *a) return (char*)s;
            a++;
        }
        s++;
    }
    return NULL;
}