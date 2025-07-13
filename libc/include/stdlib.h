#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

__attribute__((__noreturn__))
void abort(void);
void itoa(int value, char* str, int base);

#endif