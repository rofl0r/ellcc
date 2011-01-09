#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

uid_t geteuid(void)
{
    return INLINE_SYSCALL(geteuid, 0);
}
