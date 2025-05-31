#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

void init_idt();
void idt_set_gate(uint8_t num, uint32_t offset, uint16_t sel, uint8_t flags);

extern void load_idt(struct idt_ptr *ptr);

#endif