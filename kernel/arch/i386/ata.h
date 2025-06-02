#ifndef ATA_H
#define ATA_H

#include <stdint.h>

// Порты Primary IDE Channel
#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_SECT_COUNT  0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7
#define ATA_ALT_STATUS  0x3F6

// Статусные биты
#define ATA_SR_BSY      0x80
#define ATA_SR_DRDY     0x40
#define ATA_SR_DRQ      0x08
#define ATA_SR_ERR      0x01

// Команды
#define ATA_CMD_READ    0x20
#define ATA_CMD_WRITE   0x30

void ata_init();
void ata_handler(void);
void ata_read_sectors(uint32_t lba, uint16_t count, void* buffer);
void ata_write_sectors(uint32_t lba, uint16_t count, void* buffer);

extern volatile int g_ata_irq_triggered;

#endif