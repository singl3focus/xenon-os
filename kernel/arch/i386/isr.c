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
    simple_itoa(r->int_no, buf); // TODO: change
    
    terminal_writestring(buf);
    terminal_putchar('\n');
    
    // Для критических ошибок - остановка системы
    if(r->int_no == 8 || r->int_no == 13 || r->int_no == 14) {
        terminal_writestring("System Halted!\n");
        for(;;) asm volatile("hlt");
    }
}
