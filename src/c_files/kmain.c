#include "fb.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"
#include "keyboard.h"

void kmain() {
    fb_clear(); // * it's provide some not default work now, but in the future - we will fix it
    // serial_init(); // not implemented
    
    init_idt();
    pic_remap();
    
    keyboard_init(); // not tested
    
    asm volatile("sti"); // Включить прерывания

    fb_write("Welcome to Xenon-OS!\n\n", 22);
    fb_write("Enter some: ", 12);

    // serial_write("Hello, Serial Port!", 19);

    while(1);
}
