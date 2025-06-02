#include <kernel/drivers/io.h>
#include <kernel/drivers/serial.h>

#define SERIAL_COM1_BASE                0x3F8
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB         0x80

void serial_configure_baud_rate(unsigned short com, unsigned short divisor) {
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

void serial_configure_line(unsigned short com) {
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);  // 8 бит, без паритета, 1 стоп-бит
}

void serial_configure_fifo(unsigned short com) {
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);  // Включить FIFO, 14-байтовый буфер
}

void serial_configure_modem(unsigned short com) {
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);  // Включить DTR и RTS
}

int serial_is_transmit_fifo_empty(unsigned int com) {
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20; // Бит 5: Готовность к передаче
}

// Инициализация последовательного порта
void serial_init() {
    serial_configure_baud_rate(SERIAL_COM1_BASE, 3); // 38400 бод (115200 / 3)
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_fifo(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);
}

// Вывод одного символа
void serial_putc(char c) {
    while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));
    outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), c);
    
    // Добавляем символ новой строки после возврата каретки
    if (c == '\n') {
        serial_putc('\r');
    }
}

// Вывод строки
int serial_write(const char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        serial_putc(buf[i]);
    }
    return len;
}

// Вывод строки (удобная обёртка)
void serial_puts(const char *str) {
    while (*str) {
        serial_putc(*str++);
    }
}

// Вывод шестнадцатеричного числа
void serial_put_hex(uint32_t num) {
    serial_puts("0x");
    
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (num >> i) & 0xF;
        char c = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
        serial_putc(c);
    }
}

// Вывод десятичного числа
void serial_put_dec(uint32_t num) {
    if (num == 0) {
        serial_putc('0');
        return;
    }
    
    char buffer[12];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        serial_putc(buffer[j]);
    }
}