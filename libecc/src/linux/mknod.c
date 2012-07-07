#include <syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

int mknod(const char *name, mode_t mode, dev_t dev)
{
    return INLINE_SYSCALL(mknod, 3, name, mode, dev);
}
