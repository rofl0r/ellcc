#include <syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

int mkdir(const char *name, mode_t mode)
{
    return INLINE_SYSCALL(mkdir, 2, name, mode);
}
