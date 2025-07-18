#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_handler();
void timer_init();
void delay(uint32_t ticks);
uint32_t get_timer_ticks();

#endif
