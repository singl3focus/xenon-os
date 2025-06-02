#ifndef SERIAL_H
#define SERIAL_H
#include <stdint.h>

/** serial_write:
 *  Writes the contents of the buffer buf of length len to the serial port.
 *
 *  @param buf The buffer to write
 *  @param len The length of the buffer
 */

// Инициализация COM1
void serial_init();

// Вывод строки
int serial_write(const char *buf, unsigned int len);

// Вывод одного символа
void serial_putc(char c);

// Вывод строки (удобная обёртка)
void serial_puts(const char *str);

// Вывод числа в шестнадцатеричном формате
void serial_put_hex(uint32_t num);

// Вывод числа в десятичном формате
void serial_put_dec(uint32_t num);

#endif