#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int fstat(int fd, struct stat* buf)
{
    return INLINE_SYSCALL(fstat, 2, fd, buf);
}
