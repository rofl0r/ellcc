#include <syscall.h>
#include <sys/stat.h>
#include <unistd.h>

int chmod(const char* path, mode_t mode)
{
    return INLINE_SYSCALL(chmod, 2, path, mode);
}
