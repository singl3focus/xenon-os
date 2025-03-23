#include "idt.h"

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

    // Обнуление IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    // Здесь позже добавьте обработчики прерываний

    load_idt(&idtp);
}
