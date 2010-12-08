#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int _stat(const char* path, struct stat* buf)
{
    return INLINE_SYSCALL(stat, 2, path, buf);
}
