#include <syscall.h>
#include <sys/stat.h>
#include <fcntl.h>

int creat(const char *name, mode_t mode)
{
    return INLINE_SYSCALL(creat, 2, name, mode);
}
