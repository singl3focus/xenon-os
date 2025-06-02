#include <kernel/tty.h>
#include <kernel/drivers/serial.h>
#include <string.h>
#include <stdlib.h>
#include "ata.h"
#include "fat16.h"

static fat16_boot_sector_t g_bs;
static uint32_t g_partition_start;
static uint32_t g_fat_start;
static uint32_t g_root_dir_start;
static uint32_t g_data_start;

void fat16_init(uint32_t partition_start) {
    g_partition_start = partition_start;

    char buf[32];
    itoa(partition_start, buf, 10);
    serial_puts("Reading FAT16 boot sector at LBA: ");
    serial_puts(buf);
    serial_puts("\n");
    
    // Чтение boot-сектора
    ata_read_sectors(partition_start, 1, &g_bs);
    
    // Расчет позиций
    g_fat_start = partition_start + g_bs.reserved_sectors;
    g_root_dir_start = g_fat_start + (g_bs.fat_copies * g_bs.sectors_per_fat);
    g_data_start = g_root_dir_start + ((g_bs.root_entries * 32) / g_bs.bytes_per_sector);
}

int fat16_open(const char* path, void* buffer) {
    // Пока просто заглушка
    return -1;
}

void fat16_list_root() {
    uint8_t sector_buffer[512];
    uint16_t entries_per_sector = 512 / sizeof(fat16_dir_entry_t);
    
    for (uint32_t i = 0; i < g_bs.root_entries; i += entries_per_sector) {
        uint32_t sector = g_root_dir_start + (i / entries_per_sector);
        ata_read_sectors(sector, 1, sector_buffer);
        
        fat16_dir_entry_t* entry = (fat16_dir_entry_t*)sector_buffer;
        for (int j = 0; j < entries_per_sector; j++, entry++) {
            
            if (entry->filename[0] == 0x00) break; // Конец каталога
            if (entry->filename[0] == 0xE5) continue; // Удаленный файл
            
            // Вывод имени
            for (int k = 0; k < 8; k++) {
                if (entry->filename[k] != ' ') 
                    terminal_putchar(entry->filename[k]);
            }
            
            if (entry->ext[0] != ' ') {
                terminal_putchar('.');
                for (int k = 0; k < 3; k++) {
                    if (entry->ext[k] != ' ')
                        terminal_putchar(entry->ext[k]);
                }
            }
            terminal_putchar('\n');
        }
    }
}