#include <string.h>

size_t strspn(const char* s, const char* accept) {
    const char* p;
    size_t count = 0;
    
    while (*s) {
        for (p = accept; *p; p++) {
            if (*s == *p) break;
        }
        if (!*p) return count;
        count++;
        s++;
    }
    return count;
}