#include <sysdep.h>
#include <unistd.h>

int _read(int fd, const void* buf, size_t count)
{
    return INLINE_SYSCALL(read, 3, fd, buf, count);
}
