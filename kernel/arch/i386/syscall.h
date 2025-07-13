#define SYS_OPEN  1
#define SYS_READ  2
#define SYS_CLOSE 3

int sys_open(const char* path);
int sys_read(int fd, void* buf, size_t size);
void sys_close(int fd);