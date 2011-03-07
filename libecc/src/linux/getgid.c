#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

gid_t getgid(void)
{
    return INLINE_SYSCALL(getgid, 0);
}
