#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>


int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
size_t strlen(const char*);
char* strcpy(char* __restrict dest, const char* __restrict src);
int strcasecmp(const char* s1, const char* s2);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
char* strtok(char* str, const char* delim);
size_t strspn(const char* s, const char* accept);
char* strpbrk(const char* s, const char* accept);

#endif