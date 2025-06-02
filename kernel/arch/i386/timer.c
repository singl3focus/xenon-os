#include "timer.h"
#include <kernel/drivers/io.h>
#include "idt.h"
#include "irq.h"

static volatile uint32_t timer_ticks = 0;

void timer_callback() {
    timer_ticks++;
}

void delay(uint32_t ticks) {
    uint32_t start = timer_ticks;
    while ((timer_ticks - start) < ticks) {
        __asm__ volatile("hlt");
    }
}

void timer_init(uint32_t frequency) {
    // Регистрация IRQ0 обработчика
    irq_register_handler(0, timer_callback);

    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);             // Command byte: channel 0, lobyte/hibyte, mode 3
    outb(0x40, divisor & 0xFF);   // Low byte
    outb(0x40, divisor >> 8);     // High byte
}
