#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

off_t _lseek(int fd, off_t offset, int whence)
{
    return INLINE_SYSCALL(lseek, 3, fd, offset, whence);
}
