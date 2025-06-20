#include <kernel/drivers/serial.h>
#include <string.h>
#include <stdlib.h>
#include "fb.h"
#include "ata.h"
#include "fat16.h"

#define MAX_OPEN_FILES 16
static fat16_open_file_t g_open_files[MAX_OPEN_FILES];

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
                    fb_write(entry->name[k]);
            }
            
            if (entry->ext[0] != ' ') {
                fb_write('.');
                for (int k = 0; k < 3; k++) {
                    if (entry->ext[k] != ' ')
                        fb_write(entry->ext[k]);
                }
            }
            fb_write("\n");
        }
    }
}

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

int fat16_open(const char* path) {
    fat16_dir_entry_t entry;
    if (!fat16_find_file(path, &entry)) {
        return -1; // Не найден
    }

    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!g_open_files[i].used) {
            g_open_files[i].used = 1;
            g_open_files[i].entry = entry;
            g_open_files[i].position = 0;
            g_open_files[i].current_cluster = entry.start_cluster;
            return i;
        }
    }

    return -1; // Нет свободных дескрипторов
}

void fat16_close(int fd) {
    if (fd >= 0 && fd < MAX_OPEN_FILES) {
        g_open_files[fd].used = 0;
    }
}


uint16_t fat16_get_next_cluster(uint16_t cluster) {
    uint32_t fat_offset = cluster * 2;
    uint32_t fat_sector = g_fat_start + (fat_offset / g_bs.bytes_per_sector);
    uint16_t offset = fat_offset % g_bs.bytes_per_sector;

    uint8_t sector[g_bs.bytes_per_sector];
    ata_read_sectors(fat_sector, 1, sector);

    return *(uint16_t*)&sector[offset];
}


int fat16_read(int fd, void* buf, size_t size) {
    if (fd < 0 || fd >= MAX_OPEN_FILES || !g_open_files[fd].used)
        return -1;

    fat16_open_file_t* file = &g_open_files[fd];
    uint8_t* out = (uint8_t*)buf;

    uint32_t file_size = file->entry.file_size;
    uint32_t bytes_left = file_size - file->position;
    if (size > bytes_left) size = bytes_left;

    uint32_t cluster = file->current_cluster;
    uint32_t offset = file->position;
    uint32_t bytes_read = 0;

    while (size > 0 && cluster < 0xFFF8) {
        uint32_t cluster_sector = g_data_start + ((cluster - 2) * g_bs.sectors_per_cluster);
        uint32_t cluster_offset = offset % (g_bs.bytes_per_sector * g_bs.sectors_per_cluster);
        uint32_t start_sector = cluster_sector + (cluster_offset / g_bs.bytes_per_sector);
        uint32_t sector_offset = cluster_offset % g_bs.bytes_per_sector;

        uint8_t sector[512];
        ata_read_sectors(start_sector, 1, sector);

        uint32_t to_copy = g_bs.bytes_per_sector - sector_offset;
        if (to_copy > size) to_copy = size;

        memcpy(out, sector + sector_offset, to_copy);
        out += to_copy;
        bytes_read += to_copy;
        offset += to_copy;
        size -= to_copy;

        // Переход на следующий кластер при необходимости
        if ((offset % (g_bs.bytes_per_sector * g_bs.sectors_per_cluster)) == 0) {
            cluster = fat16_get_next_cluster(cluster);
            file->current_cluster = cluster;
        }
    }

    file->position += bytes_read;
    return bytes_read;
}

int fat16_read_file(const char* path, void* buf, size_t max_size) {
    int fd = fat16_open(path);
    if (fd < 0) return -1;
    int len = fat16_read(fd, buf, max_size);
    fat16_close(fd);
    return len;
}
