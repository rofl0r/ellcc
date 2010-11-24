#include <sysdep.h>
#include <unistd.h>
#include <sys/stat.h>

int fchmod(int fd, mode_t mode)
{
    return INLINE_SYSCALL(fchmod, 2, fd, mode);
}
