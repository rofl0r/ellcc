#include <sysdep.h>
#include <sys/types.h>
#include <libc-symbols.h>

pid_t _getpid(void)
{
    return INLINE_SYSCALL(getpid, 0);
}
