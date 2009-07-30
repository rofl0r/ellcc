#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>

int _write(int fd, const void* buf, size_t count)
{
    return INLINE_SYSCALL(write, 3, fd, buf, count);
}
