#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "syscall.h"
#include "fat16.h"
#include "copy.h"  
#include "errno.h"  

typedef uint32_t (*sc_wrapper_t)(uint32_t a, uint32_t b, uint32_t c);

// Реальные реализации уровня ядра (не вызываются напрямую из таблицы)
static int ksys_open(const char *kpath) {
    if (!kpath) return -EFAULT;
    int fd = fat16_open(kpath);
    return fd >= 0 ? fd : -ENOENT; 
}

static int ksys_read(int fd, void *kbuf, size_t size) {
    if (!kbuf) return -EFAULT;
    int got = fat16_read(fd, kbuf, size);
    return got; 
}

static int ksys_close(int fd) {
    fat16_close(fd);
    return 0;
}

static uint32_t wrap_open(uint32_t a, uint32_t b, uint32_t c) {
    const char *user_path = (const char*)a;
    char kpath[256];
    if (copy_from_user(kpath, user_path, sizeof(kpath)-1) < 0) return (uint32_t)-EFAULT;
    kpath[sizeof(kpath)-1] = '\0';
    int fd = ksys_open(kpath);
    return (uint32_t)fd;
}

static uint32_t wrap_read(uint32_t a, uint32_t b, uint32_t c) {
    int fd = (int)a;
    void *user_buf = (void*)b;
    size_t size = (size_t)c;
    if (size == 0) return 0;

    void *kbuf = kmalloc(size);
    if (!kbuf) return (uint32_t)-ENOMEM;

    int got = ksys_read(fd, kbuf, size);
    if (got < 0) {
        kfree(kbuf);
        return (uint32_t)got;
    }

    if (copy_to_user(user_buf, kbuf, (size_t)got) < 0) {
        kfree(kbuf);
        return (uint32_t)-EFAULT;
    }

    kfree(kbuf);
    return (uint32_t)got;
}

static uint32_t wrap_close(uint32_t a, uint32_t b, uint32_t c) {
    int fd = (int)a;
    int r = ksys_close(fd);
    return (uint32_t)r;
}

static sc_wrapper_t syscall_table[] = {
    0,              /* 0 — not used */
    wrap_open,      /* 1 SYS_OPEN */
    wrap_read,      /* 2 SYS_READ */
    wrap_close      /* 3 SYS_CLOSE */
};

#define SYSCALL_COUNT (sizeof(syscall_table)/sizeof(syscall_table[0]))

/* Диспетчер: ожидаем num в eax, arg1..3 в ebx/ecx/edx (или что совпадает с asm-stub) */
int32_t syscall_handler(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    if (num == 0 || num >= SYSCALL_COUNT || syscall_table[num] == NULL) {
        return -ENOSYS;
    }
    uint32_t res = syscall_table[num](arg1, arg2, arg3);
    return (int32_t)res;
}
