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
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attr;
    uint8_t reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t high_start_cluster; // = 0 for FAT16
    uint16_t write_time;
    uint16_t write_date;
    uint16_t start_cluster;
    uint32_t file_size;
} __attribute__((packed)) fat16_dir_entry_t;
#pragma pack(pop)

void fat16_init(uint32_t partition_start);
int fat16_open(const char* path, void* buffer);
void fat16_list_root();

#endif