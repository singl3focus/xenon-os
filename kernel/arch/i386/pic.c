#include <kernel/drivers/io.h>

#include "pic.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

void pic_remap(int offset1, int offset2) {
    // Сохраняем маски
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    // Инициализация
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // Смещения
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);
    
    // Сообщаем о наличии ведомого PIC
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    
    // Режим 8086
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Восстанавливаем маски
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq) {
    if(irq >= 8) outb(PIC2_COMMAND, 0x20);
    outb(PIC1_COMMAND, 0x20);
}