#ifndef TYPES_H
#define TYPES_H

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;

typedef unsigned int       uintptr_t;

typedef struct {
    uintptr_t address;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;  // Бит на пиксель
} Framebuffer_Info;

#endif // TYPES_H
