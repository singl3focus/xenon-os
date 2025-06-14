#include <stdint.h>
#include <stddef.h>
#include "syscall.h"
#include "fat16.h"

typedef uint32_t (*syscall_func_t)(uint32_t, uint32_t, uint32_t);

int sys_open(const char* path) {
    return fat16_open((const char*)path);
}

int sys_read(int fd, void* buf, size_t size) {
    return fat16_read(fd, buf, size);
}

void sys_close(int fd) {
    fat16_close(fd);
}

static syscall_func_t syscall_table[] = {
    0,                  // 0 — не используется
    (syscall_func_t)sys_open,  // 1
    (syscall_func_t)sys_read,  // 2
    (syscall_func_t)sys_close  // 3
};

uint32_t syscall_handler(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    if (num >= sizeof(syscall_table) / sizeof(syscall_func_t) || syscall_table[num] == 0)
        return (uint32_t)-1;

    return syscall_table[num](arg1, arg2, arg3);
}
