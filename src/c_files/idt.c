#include "idt.h"
#include "isr.h"
#include "irq.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void load_idt(struct idt_ptr *ptr);

void idt_set_gate(uint8_t num, uint32_t offset, uint16_t sel, uint8_t flags) {
    idt[num].offset_low = offset & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
    idt[num].offset_high = (offset >> 16) & 0xFFFF;
}

void init_idt() {
    idtp.limit = sizeof(struct idt_entry) * 256 - 1;
    idtp.base = (uint32_t)&idt;

    // Исключения (0-31)
    idt_set_gate(0, (uint32_t)&isr0, 0x08, 0x8E);   // Деление на ноль
    idt_set_gate(8, (uint32_t)&isr8, 0x08, 0x8E);   // Double Fault
    idt_set_gate(13, (uint32_t)&isr13, 0x08, 0x8E); // General Protection Fault
    // TODO: добавить остальные исключения

    idt_set_gate(32, (uint32_t)&irq0, 0x08, 0x8E);  // IRQ0 (Таймер)
    idt_set_gate(33, (uint32_t)&irq1, 0x08, 0x8E);  // IRQ1 (Клавиатура)
    idt_set_gate(34, (uint32_t)&irq2, 0x08, 0x8E);  
    idt_set_gate(35, (uint32_t)&irq3, 0x08, 0x8E);  
    idt_set_gate(36, (uint32_t)&irq4, 0x08, 0x8E);  
    idt_set_gate(37, (uint32_t)&irq5, 0x08, 0x8E);  
    idt_set_gate(38, (uint32_t)&irq6, 0x08, 0x8E);  
    idt_set_gate(39, (uint32_t)&irq7, 0x08, 0x8E);  
    idt_set_gate(40, (uint32_t)&irq8, 0x08, 0x8E);  
    idt_set_gate(41, (uint32_t)&irq9, 0x08, 0x8E);  
    idt_set_gate(42, (uint32_t)&irq10, 0x08, 0x8E);  
    idt_set_gate(43, (uint32_t)&irq11, 0x08, 0x8E);  
    idt_set_gate(44, (uint32_t)&irq12, 0x08, 0x8E);  
    idt_set_gate(45, (uint32_t)&irq13, 0x08, 0x8E);  
    idt_set_gate(46, (uint32_t)&irq14, 0x08, 0x8E);  
    idt_set_gate(47, (uint32_t)&irq15, 0x08, 0x8E); 

    // Аппаратные прерывания (IRQ0-IRQ15 → 32-47)
    // for (int i = 0; i < 16; i++) {
    //    idt_set_gate(32 + i, (uint32_t)irq0 + i * 8, 0x08, 0x8E);
    //}

    load_idt(&idtp);
}
