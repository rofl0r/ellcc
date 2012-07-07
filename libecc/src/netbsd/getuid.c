#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

uid_t getuid(void)
{
    return INLINE_SYSCALL(getuid, 0);
}
