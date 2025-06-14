#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

void parse_multiboot2(uint32_t magic, uint32_t addr);

// Multiboot2 magic number
#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

// Multiboot2 tag types
#define MULTIBOOT_TAG_TYPE_END              0
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER      8

// Общая структура тега
struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

// Структура для framebuffer тега
struct multiboot_tag_framebuffer {
    struct multiboot_tag common;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    uint16_t reserved;
};

#endif // MULTIBOOT2_H
