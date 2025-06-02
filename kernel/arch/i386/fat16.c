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
            if (entry->name[0] == 0x00) {
                // Прекращаем чтение при пустой записи
                return;
            }

            // if (entry->filename[0] == 0x00) break; // Конец каталога
            if (entry->name[0] == 0xE5) continue; // Удаленный файл
            
            // Вывод имени
            for (int k = 0; k < 8; k++) {
                if (entry->name[k] != ' ') 
                    terminal_putchar(entry->name[k]);
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

#include <string.h>

int fat16_find_file(const char* name, fat16_dir_entry_t* out_entry) {
    uint16_t entries = g_bs.root_entries;
    uint16_t entries_per_sector = g_bs.bytes_per_sector / sizeof(fat16_dir_entry_t);
    uint16_t root_dir_sectors = ((entries * 32) + g_bs.bytes_per_sector - 1) / g_bs.bytes_per_sector;
    uint32_t root_dir_lba = g_root_dir_start;

    fat16_dir_entry_t sector_entries[entries_per_sector];

    for (uint16_t sector = 0; sector < root_dir_sectors; sector++) {
        ata_read_sectors(root_dir_lba + sector, 1, sector_entries);

        for (uint16_t i = 0; i < entries_per_sector; i++) {
            fat16_dir_entry_t* entry = &sector_entries[i];

            if (entry->name[0] == 0x00) return 0;           // Конец каталога
            if (entry->name[0] == 0xE5) continue;           // Удалённый
            if ((entry->attr & 0x0F) == 0x0F) continue;     // LFN

            char full_name[13] = {0};

            memcpy(full_name, entry->name, 8);
            for (int j = 7; j >= 0 && full_name[j] == ' '; j--) full_name[j] = '\0';
            if (entry->ext[0] != ' ') {
                strcat(full_name, ".");
                strncat(full_name, (char*)entry->ext, 3);
            }

            if (strcasecmp(name, full_name) == 0) {
                memcpy(out_entry, entry, sizeof(fat16_dir_entry_t));
                return 1;
            }
        }
    }

    return 0;
}

uint16_t fat16_get_next_cluster(uint16_t cluster) {
    uint32_t fat_offset = cluster * 2;
    uint32_t fat_sector = g_fat_start + (fat_offset / g_bs.bytes_per_sector);
    uint16_t offset = fat_offset % g_bs.bytes_per_sector;

    uint8_t sector[g_bs.bytes_per_sector];
    ata_read_sectors(fat_sector, 1, sector);

    return *(uint16_t*)&sector[offset];
}


int fat16_read_file(const fat16_dir_entry_t* entry, void* buffer, uint32_t max_size) {
    uint16_t cluster = entry->start_cluster;
    uint32_t file_size = entry->file_size;
    uint8_t* buf = (uint8_t*)buffer;

    uint32_t bytes_read = 0;

    while (cluster < 0xFFF8 && bytes_read < file_size && bytes_read < max_size) {
        uint32_t cluster_sector = g_data_start + ((cluster - 2) * g_bs.sectors_per_cluster);

        for (int i = 0; i < g_bs.sectors_per_cluster; i++) {
            ata_read_sectors(cluster_sector + i, 1, buf);
            buf += g_bs.bytes_per_sector;
            bytes_read += g_bs.bytes_per_sector;
            if (bytes_read >= file_size || bytes_read >= max_size) break;
        }

        cluster = fat16_get_next_cluster(cluster);
    }

    return bytes_read;
}
