#include "isr.h"
#include "fb.h"

void fb_write_hex(uint32_t num) {
    char hex_buf[9];
    const char *hex_chars = "0123456789ABCDEF";
    hex_buf[0] = '0';
    hex_buf[1] = 'x';
    for (int i = 7; i >= 2; i--) {
        hex_buf[i] = hex_chars[num & 0xF];
        num >>= 4;
    }
    hex_buf[8] = '\0';
    fb_write(hex_buf, 9); // Длина 8
}

void isr_handler(ISR_Registers regs) {
    fb_write("INT: ", 5);
    fb_write_hex(regs.int_no);

    if (regs.err_code != 0xFFFFFFFF) {
        fb_write(" ERR: ", 6);
        fb_write_hex(regs.err_code);
    }

    fb_write("\n", 1);
}
