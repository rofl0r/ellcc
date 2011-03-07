#include <syscall.h>
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count)
{
    return INLINE_SYSCALL(read, 3, fd, buf, count);
}
