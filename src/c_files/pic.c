#include "io.h"
#include "pic.h"

#define PIC1 0x20
#define PIC2 0xA0

void pic_remap() {
    // ICW1
    outb(PIC1, 0x11);
    outb(PIC2, 0x11);
    
    // ICW2
    outb(PIC1 + 1, 0x20); // IRQ 0-7 -> INT 32-39
    outb(PIC2 + 1, 0x28); // IRQ 8-15 -> INT 40-47
    
    // ICW3
    outb(PIC1 + 1, 0x04);
    outb(PIC2 + 1, 0x02);
    
    // ICW4
    outb(PIC1 + 1, 0x01);
    outb(PIC2 + 1, 0x01);
    
    // Маскируем все прерывания
    outb(PIC1 + 1, 0x0);
    outb(PIC2 + 1, 0x0);
}


void pic_clear_mask(uint8_t irq) {
    uint16_t port;
    if(irq < 8) {
        port = 0x21;
    } else {
        port = 0xA1;
        irq -= 8;
    }
    outb(port, inb(port) & ~(1 << irq));
}