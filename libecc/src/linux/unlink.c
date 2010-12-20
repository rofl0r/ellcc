#include <syscall.h>
#include <unistd.h>

int unlink(const char* path)
{
    return INLINE_SYSCALL(unlink, 1, path);
}
