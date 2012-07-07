#include <syscall.h>
#include <unistd.h>

int rmdir(const char *pathname)
{
    return INLINE_SYSCALL(rmdir, 1, pathname);
}
