#include "inters.h"
#include "pic.h"

// void interrupt_handler(cpu_state cpu, stack_state stack, uint32_t intr_num) {
//     // Вывод информации о прерывании (пример)
//     fb_write("Interrupt: ");
//     fb_write_hex(intr_num);
    
//     // Использование stack.error_code (если есть)
//     if (intr_num == 8 || intr_num == 10 || intr_num == 14) { 
//         fb_write(", Error code: ");
//         fb_write_hex(stack.error_code);
//     }
    
//     // Подтверждение PIC
//     if (intr_num >= 0x20 && intr_num < 0x30) {
//         pic_acknowledge(intr_num);
//     }

//     (void)cpu;
//     (void)stack;
// }
