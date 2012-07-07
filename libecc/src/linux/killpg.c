#include <syscall.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

int killpg(pid_t pgrp, int sig)
{
    // RICH: Linux uses kill() with a negative argument.  Check other OSes.
    if ((int)pgrp < 0) {
        __set_errno(EINVAL);
        return -1;
    }
    return INLINE_SYSCALL(kill, 2, -pgrp, sig);
}
