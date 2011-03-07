#include <syscall.h>
#include <unistd.h>

ssize_t readlink(const char * __restrict path, char * __restrict buf, size_t bufsize)
{
    return INLINE_SYSCALL(readlink, 3, path, buf, bufsize);
}
