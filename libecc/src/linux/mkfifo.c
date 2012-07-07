#include <syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

int mkfifo(const char *name, mode_t mode)
{
    return INLINE_SYSCALL(mknod, 3, name, mode | S_IFIFO, 0);
}
