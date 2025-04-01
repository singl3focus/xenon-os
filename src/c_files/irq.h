#ifndef IRQ_H
#define IRQ_H

#include "types.h"

extern void irq0(void) __asm__("irq0");
extern void irq1(void) __asm__("irq1");
extern void irq2(void) __asm__("irq2");
extern void irq3(void) __asm__("irq3");;
extern void irq4(void) __asm__("irq4");;
extern void irq5(void) __asm__("irq5");
extern void irq6(void) __asm__("irq6");;
extern void irq7(void) __asm__("irq7");;
extern void irq8(void) __asm__("irq8");;
extern void irq9(void) __asm__("irq9");;
extern void irq10(void) __asm__("irq10");;
extern void irq11(void) __asm__("irq11");;
extern void irq12(void) __asm__("irq12");;
extern void irq13(void) __asm__("irq13");;
extern void irq14(void) __asm__("irq14");;
extern void irq15(void) __asm__("irq15");;

// Структура для передачи данных в C-обработчик
typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} Registers;

// Объявление C-обработчика
void irq_handler_c(Registers regs);

#endif