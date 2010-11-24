#include <sysdep.h>
#include <unistd.h>

int _unlink(const char* path)
{
    return INLINE_SYSCALL(unlink, 1, path);
}
