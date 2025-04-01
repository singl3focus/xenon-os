#pragma once

#include "idt.h"

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, ebp, esp, eip, eflags;
    uint32_t cs, ss, ds, es, fs, gs;
} cpu_state;

typedef struct {
    uint32_t error_code;
    cpu_state cpu;
} stack_state;

void interrupt_handler(cpu_state cpu, stack_state stack, uint32_t intr_num);
