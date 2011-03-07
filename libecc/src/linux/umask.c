#include <syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

mode_t umask(mode_t mask)
{
    return INLINE_SYSCALL(umask, 1, (int)mask);
}
