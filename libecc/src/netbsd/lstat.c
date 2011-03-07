#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int lstat(const char* path, struct stat* buf)
{
    return INLINE_SYSCALL(lstat, 2, path, buf);
}
