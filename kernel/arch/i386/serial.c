#include <kernel/drivers/io.h>

#define SERIAL_COM1_BASE                0x3F8
#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB         0x80

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(unsigned short com, unsigned short divisor) {
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
    outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

/** serial_configure_line:
 *  Configures the line of the given serial port.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(unsigned short com) {
    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

/** serial_configure_fifo:
 *  Configures the FIFO buffer of the given serial port.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_fifo(unsigned short com) {
    outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

/** serial_configure_modem:
 *  Configures the modem of the given serial port.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_modem(unsigned short com) {
    outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not.
 *
 *  @param com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *          1 if the transmit FIFO queue is empty
 */
int serial_is_transmit_fifo_empty(unsigned int com) {
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

/** serial_write:
 *  Writes the contents of the buffer buf of length len to the serial port.
 *
 *  @param buf The buffer to write
 *  @param len The length of the buffer
 */
int serial_write(char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        while (!serial_is_transmit_fifo_empty(SERIAL_COM1_BASE));
        outb(SERIAL_DATA_PORT(SERIAL_COM1_BASE), buf[i]);
    }
    
    return len;
}

