#include <string.h>  
#include "copy.h"
#include "errno.h"

int copy_from_user(void *dst, const void *src, size_t n) {
    if (!dst || !src) return -EFAULT;
    // TODO: здесь должна быть проверка валидности src как user-space указателя
    memcpy(dst, src, n);
    return 0;
}

int copy_to_user(void *dst, const void *src, size_t n) {
    if (!dst || !src) return -EFAULT;
    // TODO: здесь должна быть проверка валидности dst как user-space указателя
    memcpy(dst, src, n);
    return 0;
}
