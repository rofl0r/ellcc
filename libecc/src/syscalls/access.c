#include <syscall.h>
#include <unistd.h>

int access(const char *name, int mode)
{
    return INLINE_SYSCALL(access, 2, name, mode);
}
