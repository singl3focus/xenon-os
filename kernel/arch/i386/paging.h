#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGE_SIZE 4096
#define PAGE_DIR_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

// Структура записи в таблице страниц
typedef struct {
    uint32_t present    : 1;   // Страница в памяти
    uint32_t rw         : 1;   // 0=Только чтение, 1=Чтение/запись
    uint32_t user       : 1;   // 0=Супервайзер, 1=Пользователь
    uint32_t accessed   : 1;   // Страница была прочитана
    uint32_t dirty      : 1;   // Страница была изменена
    uint32_t unused     : 7;   // Не используются
    uint32_t frame      : 20;  // Адрес фрейма (сдвинутый на 12 бит)
} page_table_entry_t;

// Каталог страниц (содержит указатели на таблицы)
typedef struct {
    page_table_entry_t *tables[PAGE_DIR_ENTRIES];  // Виртуальные адреса
    uint32_t tables_physical[PAGE_DIR_ENTRIES];    // Физические адреса
    uint32_t physical_addr;                        // Физический адрес PD
} page_directory_t;

// Инициализация пейджинга
void paging_init();

// Включение пейджинга (ассемблерная функция)
extern void switch_page_directory(uint32_t pd_physical);

#endif