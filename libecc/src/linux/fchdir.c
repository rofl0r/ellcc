#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int fchdir(int fd)
{
    return INLINE_SYSCALL(fchdir, 1, fd);
}
