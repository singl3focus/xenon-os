#ifndef FB_H
#define FB_H

/** fb_write:
 *  Writes the contents of the buffer buf of length len to the screen.
 *
 *  @param buf The buffer to write
 *  @param len The length of the buffer
 */
int fb_write(char *buf, unsigned int len);

void fb_clear(void);

#endif
