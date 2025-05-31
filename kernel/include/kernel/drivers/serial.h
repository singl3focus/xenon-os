#ifndef SERIAL_H
#define SERIAL_H

/** serial_write:
 *  Writes the contents of the buffer buf of length len to the serial port.
 *
 *  @param buf The buffer to write
 *  @param len The length of the buffer
 */
int serial_write(char *buf, unsigned int len);

#endif