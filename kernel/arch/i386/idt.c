#include "idt.h"
#include "isr.h"
#include "irq.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = sel;
    idt[num].zero = 0;
    idt[num].flags = flags | IDT_PRESENT;
}

void idt_init() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint32_t)&idt;
    
    // Обнуление IDT
    for(int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // 
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E); // Деление на ноль
    idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);   
    idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);   
    idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);   
    idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);   
    idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);   
    idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);   
    idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);   
    idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);   
    idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);   
    idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);   
    idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);   
    idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);   
    idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);   
    idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);   
    idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);   
    idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);   
    idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);   
    idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);   
    idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);   
    idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);   
    idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);  
    idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);   
    
    // 
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);  // IRQ0 (Таймер)
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // IRQ1 (Клавиатура)
    idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);  
    idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);  
    idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);  
    idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);  
    idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);  
    idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);  
    idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);  
    idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);  
    idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);  
    idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);  
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);  
    idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);  
    idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);  
    idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E); 
    
    idt_set_gate(128, (uint32_t)isr128, 0x08, 0x8E); // 0x80 → syscall
    
    idt_load((uint32_t)&idtp);
}