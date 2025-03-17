#include "fb.h"
#include "serial.h"

int kmain() {
    fb_write("Hello, World!", 13);
    serial_write("Hello, Serial Port!", 19);
    return 0;
}
