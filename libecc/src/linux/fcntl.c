#include <syscall.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

int fcntl(int fd, int cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);
    struct flock* lock = va_arg(ap, struct flock*);
    va_end(ap);
    return INLINE_SYSCALL(fcntl, 3, fd, cmd, lock);
}
