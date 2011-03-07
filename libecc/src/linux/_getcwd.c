#include <syscall.h>
#include <unistd.h>

ssize_t __getcwd(char * __restrict buf, size_t size)
{
    return INLINE_SYSCALL(getcwd, 2, buf, size);
}
