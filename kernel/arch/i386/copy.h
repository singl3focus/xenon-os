// copy.h
#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * Копирует данные из пространства пользователя в ядро.
 * @param dst  — буфер в ядре
 * @param src  — указатель в пользовательской памяти
 * @param n    — количество байт
 * @return 0 при успехе, -1 при ошибке (EFAULT)
 */
int copy_from_user(void *dst, const void *src, size_t n);

/**
 * Копирует данные из ядра в пользовательское пространство.
 * @param dst  — указатель в пользовательской памяти
 * @param src  — буфер в ядре
 * @param n    — количество байт
 * @return 0 при успехе, -1 при ошибке (EFAULT)
 */
int copy_to_user(void *dst, const void *src, size_t n);
