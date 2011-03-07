#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

gid_t getegid(void)
{
    return INLINE_SYSCALL(getegid, 0);
}
