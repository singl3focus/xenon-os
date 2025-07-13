#include "timer.h"
#include <kernel/drivers/io.h>
#include "idt.h"
#include "irq.h"
#include "pic.h"

static volatile uint32_t timer_ticks = 0;

void timer_handler() {
    timer_ticks++;
}

// Функция для получения текущего времени
uint32_t get_timer_ticks() {
    return timer_ticks;
}

void delay(uint32_t ticks) {
    uint32_t start = timer_ticks;
    while ((timer_ticks - start) < ticks) {
        __asm__ volatile("hlt");
    }
}

void timer_init() {
    // Разрешаем IRQ0 в PIC
    pic_send_eoi(0);

    uint32_t frequency = 2000;
    uint32_t divisor = 1193180 / frequency;

    outb(0x43, 0x36);             // Channel 0, lobyte/hibyte, mode 3
    outb(0x40, divisor & 0xFF);   // Low byte
    outb(0x40, divisor >> 8);     // High byte
}
