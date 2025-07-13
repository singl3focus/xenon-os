#ifndef ISR_H
#define ISR_H

#include "types.h"

extern void isr0(void) __asm__("isr0");
extern void isr8(void) __asm__("isr8");
extern void isr13(void) __asm__("isr13");

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} ISR_Registers;

void isr_handler(ISR_Registers regs);

#endif