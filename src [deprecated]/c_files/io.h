#ifndef IO_H
#define IO_H

/** outb:
 *  Sends the given data to the given I/O port.
 *
 *  @param port The I/O port to send the data to
 *  @param data The data to send to the I/O port
 */
void outb(unsigned short port, unsigned char data);

/** inb:
 *  Read a byte from an I/O port.
 *
 *  @param  port The address of the I/O port
 *  @return      The read byte
 */
unsigned char inb(unsigned short port);

void outw(unsigned short port, unsigned short data);

unsigned short inw(unsigned short port);

#endif /* INCLUDE_IO_H */
