#include <kernel/drivers/io.h> // Для memset
#include <string.h>

#include "paging.h"

// Текущий каталог страниц
page_directory_t *kernel_directory = 0;

// Выделение выровненной памяти (для структур пейджинга)
static void *kmalloc_a(size_t size) {
    // Реальная реализация потребует аллокатора памяти
    // Пока используем статический буфер
    static char buffer[PAGE_SIZE * 16];
    static uint32_t alloc_ptr = 0;
    
    // Выравнивание по границе страницы
    uint32_t addr = (uint32_t)buffer + alloc_ptr;
    addr = (addr + (PAGE_SIZE-1)) & ~(PAGE_SIZE-1);
    
    alloc_ptr = addr - (uint32_t)buffer + size;
    return (void*)addr;
}

uint32_t virtual_to_physical(void* virt_addr) {
    uintptr_t addr = (uintptr_t)virt_addr;
    if(addr >= 0xC0000000 && addr < 0xC0400000) {
        return addr - 0xC0000000;  // Первые 4MB виртуальной памяти
    }
    return addr;  // Identity mapping для остальных адресов
}

void paging_init() {
    // 1. Создаем постоянные структуры пейджинга
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    
    // 2. Identity mapping для первых 4MB
    page_table_entry_t *table = (page_table_entry_t*)kmalloc_a(PAGE_SIZE);
    for (uint32_t i = 0; i < 1024; i++) {
        table[i] = (page_table_entry_t){
            .present = 1,
            .rw = 1,
            .user = 0,
            .frame = i  // Физический адрес = виртуальный
        };
    }
    
    // 3. Mapping для области ядра (0xC0000000 -> 0x00000000)
    kernel_directory->tables[768] = table; // 0xC0000000 / 4MB = 768
    kernel_directory->tables_physical[768] = virtual_to_physical(table);
    
    // 4. Identity mapping для первых 4MB
    kernel_directory->tables[0] = table;
    kernel_directory->tables_physical[0] = virtual_to_physical(table);

    // identity mapping для всей используемой памяти
    for(uint32_t i = 1; i < 256; i++) {  // Первые 1GB
        kernel_directory->tables[i] = table;
        kernel_directory->tables_physical[i] = virtual_to_physical(table);
    }
    
    // 5. Загружаем постоянный PD
    kernel_directory->physical_addr = virtual_to_physical(kernel_directory->tables_physical);
    // switch_page_directory(kernel_directory->physical_addr);
}