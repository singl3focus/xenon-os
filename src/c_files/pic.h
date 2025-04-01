#ifndef PIC_H
#define PIC_H

#include "types.h"

void pic_remap();
void pic_clear_mask(uint8_t irq);

#endif