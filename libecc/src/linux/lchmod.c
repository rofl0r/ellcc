#include <syscall.h>
#include <sys/stat.h>
#include <unistd.h>

int lchmod(const char* path, mode_t mode)
{
    return -1; // RICH: this doesn't exist for linux. return INLINE_SYSCALL(lchmod, 2, path, mode);
}
