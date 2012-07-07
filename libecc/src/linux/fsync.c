#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int fsync(int fd)
{
    return INLINE_SYSCALL(fsync, 1, fd);
}
