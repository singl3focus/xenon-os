#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Структура дескриптора IDT
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed));

// Структура для регистра IDTR
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Типы шлюзов
#define IDT_TASK_GATE 0x5
#define IDT_INTERRUPT_GATE_16 0x6
#define IDT_TRAP_GATE_16 0x7
#define IDT_INTERRUPT_GATE_32 0xE
#define IDT_TRAP_GATE_32 0xF

// Флаги доступа
#define IDT_PRESENT 0x80
#define IDT_DPL0 0x00
#define IDT_DPL3 0x60

void idt_init();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

extern void idt_load(uint32_t idt_ptr);

#endif