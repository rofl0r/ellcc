#include <syscall.h>
#include <unistd.h>

int rename(const char* oldpath, const char *newpath)
{
    return INLINE_SYSCALL(rename, 2, oldpath, newpath);
}
