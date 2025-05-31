#ifndef ISR_H
#define ISR_H

#include <stdint.h>

extern void isr0(void) __asm__("isr0");
extern void isr1(void) __asm__("isr1");
extern void isr2(void) __asm__("isr2");
extern void isr3(void) __asm__("isr3");
extern void isr4(void) __asm__("isr4");
extern void isr5(void) __asm__("isr5");
extern void isr6(void) __asm__("isr6");
extern void isr7(void) __asm__("isr7");
extern void isr8(void) __asm__("isr8");
extern void isr9(void) __asm__("isr9");
extern void isr10(void) __asm__("isr10");
extern void isr11(void) __asm__("isr11");
extern void isr12(void) __asm__("isr12");
extern void isr13(void) __asm__("isr13");
extern void isr14(void) __asm__("isr14");
extern void isr15(void) __asm__("isr15");
extern void isr16(void) __asm__("isr16");
extern void isr17(void) __asm__("isr17");
extern void isr18(void) __asm__("isr18");
extern void isr19(void) __asm__("isr19");
extern void isr20(void) __asm__("isr20");
extern void isr21(void) __asm__("isr21");
extern void isr22(void) __asm__("isr22");
extern void isr23(void) __asm__("isr23");
extern void isr24(void) __asm__("isr24");
extern void isr25(void) __asm__("isr25");
extern void isr26(void) __asm__("isr26");
extern void isr27(void) __asm__("isr27");
extern void isr28(void) __asm__("isr28");
extern void isr29(void) __asm__("isr29");
extern void isr30(void) __asm__("isr30");
extern void isr31(void) __asm__("isr31");

#ifndef REGS_DEFINED
#define REGS_DEFINED
struct regs {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
};
#endif

void isr_handler(struct regs *r);

#endif