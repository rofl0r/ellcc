#include <syscall.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdarg.h>

int ioctl(int d, unsigned long request, ...)
{
    va_list ap;
    va_start(ap, request);
    void *ptr = va_arg(ap, void *);
    va_end(ap);
    return INLINE_SYSCALL(ioctl, 3, d, request, ptr);
}
