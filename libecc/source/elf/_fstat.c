#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int _fstat(int fd, const void* buf, size_t count)
{
    return INLINE_SYSCALL(fstat, 3, fd, buf, count);
}
