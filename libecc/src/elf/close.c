#include <sysdep.h>
#include <sys/types.h>
#include <unistd.h>

int _close(int fd)
{
    return INLINE_SYSCALL(close, 1, fd);
}
