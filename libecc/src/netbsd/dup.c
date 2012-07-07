#include <syscall.h>
#include <unistd.h>

int dup(int fd)
{
    return INLINE_SYSCALL(dup, 1, fd);
}
