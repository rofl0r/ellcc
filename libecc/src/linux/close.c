#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int close(int fd)
{
    return INLINE_SYSCALL(close, 1, fd);
}
