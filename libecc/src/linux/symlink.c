#include <syscall.h>
#include <unistd.h>

int symlink(const char *oldpath, const char *newpath)
{
    return INLINE_SYSCALL(symlink, 2, oldpath, newpath);
}
