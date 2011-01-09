#include <syscall.h>
#include <sys/time.h>

int getitimer(int which, struct itimerval *curr_value)
{
    return INLINE_SYSCALL(getitimer, 2, which, curr_value);
}
