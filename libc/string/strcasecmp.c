#include <string.h>

int strcasecmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = *s1;
        char c2 = *s2;

        // Преобразуем в нижний регистр
        if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
        if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';

        if (c1 != c2)
            return (unsigned char)c1 - (unsigned char)c2;

        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}
