#include <syscall.h>
#include <unistd.h>

int chdir(const char* path)
{
    return INLINE_SYSCALL(chdir, 1, path);
}
