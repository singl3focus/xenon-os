#include <kernel/drivers/io.h>
#include <kernel/drivers/serial.h>
#include "ata.h"

// Глобальный флаг и семафор
volatile int g_ata_irq_triggered = 0;

static void ata_wait() {
    while (inb(ATA_STATUS) & ATA_SR_BSY);
}

void ata_handler(void) {
    serial_puts("ATA IRQ triggered\n");

    // Прочитать статус для сброса прерывания
    uint8_t status = inb(ATA_STATUS);

    if (status & ATA_SR_ERR) {
        // TODO Обработка ошибки
        serial_puts("ATA status error flag set!\n");
    }

    if (!(status & ATA_SR_DRQ)) {
        serial_puts("DRQ not set — no data ready\n");
        return;
    }
    
    // Обновляем флаги состояния драйвера
    // Например, снять флаг ожидания прерывания
    g_ata_irq_triggered = 1;
}

void ata_read_sectors(uint32_t lba, uint16_t count, void* buffer) {
    serial_puts("ATA read starting...\n");
    ata_wait();
    
    // Выбор устройства (LBA mode + Master)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Добавьте задержку
    for(int i=0; i<1000; i++) asm volatile("nop");
    
    outb(ATA_SECT_COUNT, count);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, ATA_CMD_READ);

    // Атомарный сброс флага ПЕРЕД отправкой команды
    asm volatile("cli");
    g_ata_irq_triggered = 0;
    outb(ATA_COMMAND, ATA_CMD_READ);
    asm volatile("sti");
    
    uint16_t* dest = (uint16_t*)buffer;
    for (uint16_t i = 0; i < count; i++) {
        int timeout = 1000000;
        
        // Ожидание с проверкой условия в атомарном блоке
        while (1) {
            asm volatile("cli");
            int triggered = g_ata_irq_triggered;
            asm volatile("sti");
            
            if (triggered) break;
            if (--timeout <= 0) {
                serial_puts("ATA IRQ timeout\n");
                return;
            }
            asm volatile("nop");
        }
        
        // Проверка DRQ перед чтением
        if (!(inb(ATA_STATUS) & ATA_SR_DRQ)) {
            serial_puts("DRQ not set!\n");
            return;
        }
        
        // Чтение данных
        for (int j = 0; j < 256; j++) {
            dest[j] = inw(ATA_DATA);
        }
        dest += 256;
        
        // Сброс флага для следующих секторов
        if (i < count - 1) {
            asm volatile("cli");
            g_ata_irq_triggered = 0;
            asm volatile("sti");
        }
    }
    serial_puts("ATA read completed\n");
}

void ata_init() { // поллинг (без прерываний)
    // Пока просто проверяем готовность
    ata_wait();
    outb(ATA_DRIVE_SEL, 0xA0); // Select Master
    while ((inb(ATA_STATUS) & ATA_SR_DRDY) == 0);
}

void ata_reset() {
    outb(ATA_ALT_STATUS, 0x04); // SRST
    for(int i=0; i<10000; i++) asm("nop");
    outb(ATA_ALT_STATUS, 0x00);
    ata_wait();
}