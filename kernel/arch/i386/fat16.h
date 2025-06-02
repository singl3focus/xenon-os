#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint8_t  jmp[3];
    char     oem[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_copies;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
} fat16_boot_sector_t;

typedef struct {
    char     filename[8];
    char     ext[3];
    uint8_t  attributes;
    uint8_t  reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster;
    uint32_t file_size;
} fat16_dir_entry_t;
#pragma pack(pop)

void fat16_init(uint32_t partition_start);
int fat16_open(const char* path, void* buffer);
void fat16_list_root();

#endif