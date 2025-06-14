#include "irq.h"
#include "io.h"
#include "pic.h"
#include "keyboard.h"

#define PIC2_CMD 0xA0
#define PIC1_CMD 0x20

// Обработчик аппаратных прерываний
void irq_handler_c(Registers regs) {
    uint8_t irq = regs.int_no - 32;

    // Подтверждение прерывания в PIC
    if (irq >= 8) {
        outb(PIC2_CMD, 0x20); // Для slave PIC (IRQ8-15)
    }
    outb(PIC1_CMD, 0x20);     // Для master PIC (IRQ0-7)

    // Обработка конкретного IRQ
    switch (irq) {
        case 0:
            // Обработка таймера (IRQ0)
            break;
        case 1: // Обработка клавиатуры (IRQ1)
            keyboard_handler();
            break;
        // ... добавьте обработку других IRQ ...
        default:
            break;
    }
}
