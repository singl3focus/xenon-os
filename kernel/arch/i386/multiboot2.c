#include <kernel/drivers/serial.h>
#include "multiboot2.h"
#include "fb.h"

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) mb2_tag;

typedef struct {
    mb2_tag tag;                    // Содержит type и size
    uint64_t framebuffer_addr;      // Физический адрес
    uint32_t framebuffer_pitch;     // Байт на строку
    uint32_t framebuffer_width;     // Ширина
    uint32_t framebuffer_height;    // Высота
    uint8_t framebuffer_bpp;        // Бит на пиксель
    uint8_t framebuffer_type;       // Тип цветового формата
    uint8_t reserved;               // Только 1 байт резерва
} __attribute__((packed)) mb2_tag_efi_fb;

void parse_multiboot2(uint32_t magic, uint32_t addr) {
    if (magic != 0x36d76289) 
    {
        serial_puts("Invalid Multiboot magic\n");
        return;
    }

    struct multiboot_tag *tag;
    uint32_t size = *(uint32_t *)addr;
    addr += 8;

    for (tag = (struct multiboot_tag *)addr;
         tag->type != MULTIBOOT_TAG_TYPE_END && (uint32_t)tag < addr + size;
         tag = (struct multiboot_tag *)((uint32_t)tag + ((tag->size + 7) & ~7))) {

        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            struct multiboot_tag_framebuffer *fb_tag = (struct multiboot_tag_framebuffer *)tag;

            fb_info.address = fb_tag->framebuffer_addr;
            fb_info.width   = fb_tag->framebuffer_width;
            fb_info.height  = fb_tag->framebuffer_height;
            fb_info.pitch   = fb_tag->framebuffer_pitch;
            fb_info.bpp     = fb_tag->framebuffer_bpp;

            return;
        }
    }

    serial_puts("No framebuffer tag found in Multiboot2 info\n");
}
