#include <syscall.h>
#include <unistd.h>

int link(const char *oldpath, const char *newpath)
{
    return INLINE_SYSCALL(link, 2, oldpath, newpath);
}
