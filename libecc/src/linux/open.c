#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

int open(const char *name, int flags, ...)
{
    va_list ap;
    va_start(ap, flags);
    mode_t mode = va_arg(ap, int);
    va_end(ap);
    return INLINE_SYSCALL(open, 3, name, flags, mode);
}
