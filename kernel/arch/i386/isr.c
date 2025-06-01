#include <kernel/tty.h>

#include "isr.h"

static void simple_itoa(uint32_t num, char* str) {
    int i = 0;
    do {
        str[i++] = num % 10 + '0';
        num /= 10;
    } while (num > 0);
    
    // Перевернуть строку
    int len = i;
    for (int j = 0; j < len/2; j++) {
        char temp = str[j];
        str[j] = str[len-j-1];
        str[len-j-1] = temp;
    }
    str[len] = '\0';
}

void isr_handler(struct regs *r) {
    terminal_writestring("Exception: ");
    char buf[10];
    simple_itoa(r->int_no, buf); // TODO change
    
    terminal_writestring(buf);
    terminal_putchar('\n');
    
    switch (r->int_no) {
    case 14: // Page Fault
        uint32_t fault_addr;
        asm volatile("mov %%cr2, %0" : "=r" (fault_addr));
        
        terminal_writestring("Page fault at ");
        char buf[16];
        simple_itoa(fault_addr, buf);
        terminal_writestring(buf);
        terminal_putchar('\n');
        
        // TODO добавить обработку
        for(;;);
        break;
    case 8:
    case 13:
        // Для критических ошибок - остановка системы
        terminal_writestring("System Halted!\n");
        for(;;) asm volatile("hlt");
        break;
    }
}
