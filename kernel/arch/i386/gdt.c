#include "gdt.h"

// Дескрипторы для кода, данных и нулевой записью
struct gdt_entry gdt[3];
struct gdt_ptr gp;

// Установка дескриптора в GDT
static void gdt_set_entry(int idx, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[idx].base_low = (base & 0xFFFF);
    gdt[idx].base_middle = (base >> 16) & 0xFF;
    gdt[idx].base_high = (base >> 24) & 0xFF;
    
    gdt[idx].limit_low = (limit & 0xFFFF);
    gdt[idx].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    
    gdt[idx].access = access;
}

// Загрузка GDT в процессор (ассемблерная команда)
extern void gdt_flush(uint32_t gp_ptr);

void gdt_init() {
    // Настройка указателя на GDT
    gp.limit = sizeof(gdt) - 1;
    gp.base = (uint32_t)&gdt;
    
    // Нулевой дескриптор (обязателен)
    gdt_set_entry(0, 0, 0, 0, 0);
    
    // Сегмент кода: базa=0, лимит=4 ГБ, доступ=0x9A (код/ядро), гранулярность=0xCF (4KБ/32-бит)
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    
    // Сегмент данных: базa=0, лимит=4 ГБ, доступ=0x92 (данные/ядро), гранулярность=0xCF
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // Загрузить GDT в процессор
    gdt_flush((uint32_t)&gp);
}