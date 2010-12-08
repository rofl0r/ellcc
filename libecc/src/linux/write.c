#include <syscall.h>
#include <unistd.h>

ssize_t write(int fd, const void* buf, size_t count)
{
    return INLINE_SYSCALL(write, 3, fd, buf, count);
}
