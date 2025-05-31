// bga.c
#include "io.h"
#include "types.h"

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_INDEX_ID     0
#define VBE_DISPI_INDEX_XRES   1
#define VBE_DISPI_INDEX_YRES   2
#define VBE_DISPI_INDEX_BPP    3
#define VBE_DISPI_INDEX_ENABLE 4
#define VBE_DISPI_INDEX_BANK   5
#define VBE_DISPI_LFB_ENABLED  0x40
#define VBE_DISPI_NOCLEARMEM   0x80
#define VBE_DISPI_ID5          0xB0C5

void bga_write_register(uint16_t index, uint16_t value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

uint16_t bga_read_register(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

void bga_init() {
    // Проверка поддержки BGA
    if (bga_read_register(VBE_DISPI_INDEX_ID) != VBE_DISPI_ID5) {
        // Ошибка: BGA не поддерживается
        return;
    }

    // Настройка разрешения 1024x768x32
    bga_write_register(VBE_DISPI_INDEX_ENABLE, 0x00); // Отключить BGA
    bga_write_register(VBE_DISPI_INDEX_XRES, 1024);
    bga_write_register(VBE_DISPI_INDEX_YRES, 768);
    bga_write_register(VBE_DISPI_INDEX_BPP, 32);
    // Включить BGA с LFB и без очистки памяти
    bga_write_register(VBE_DISPI_INDEX_ENABLE, 0x01 | VBE_DISPI_LFB_ENABLED | VBE_DISPI_NOCLEARMEM);
}