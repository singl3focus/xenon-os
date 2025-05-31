# Project

## libc

### include

#### string.h

1. memcmp - Сравнивает n байт в двух блоках памяти.
```c
int memcmp(const void*, const void*, size_t);
```

Возвращает:
- 0 если блоки идентичны,
- <0 если первый отличающийся байт в первом блоке меньше,
- >0 если больше.

2. memcpy - Копирует n байт из src в dest.
```c
void* memcpy(void* __restrict dest, const void* __restrict src, size_t n);
```

__restrict: Гарантирует отсутствие пересечения областей памяти (для оптимизации).

Поведение: Не определено при перекрытии областей (используйте memmove).

3. memmove - Копирует n байт из src в dest безопасно при перекрытии областей.
```c
void* memmove(void* dest, const void* src, size_t n);
```

Особенность: Проверяет порядок копирования (например, если dest > src, копирует с конца).

4. memset - Заполняет n байт области dest байтом ch.
```c
void* memset(void* dest, int ch, size_t n);
```

Пример: memset(arr, 0, sizeof(arr)) обнуляет массив.

5. strlen - Вычисляет длину C-строки (до первого нуль-терминатора \0).
```c
size_t strlen(const char* str);
```

Важно: Не включает \0 в результат.


> Пример использования:

```c
#include <string.h> // Этот файл

int main() {
    char src[] = "Hello";
    char dest[6];
    
    memcpy(dest, src, strlen(src)+1); // Копирование строки
    return memcmp(src, dest, 5);      // Вернет 0 (равны)
}
```