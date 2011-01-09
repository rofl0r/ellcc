#include <syscall.h>
#include <sys/time.h>

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value)
{
    return INLINE_SYSCALL(setitimer, 3, which, new_value, old_value);
}
