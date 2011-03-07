#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

pid_t getpgrp(void)
{
    return INLINE_SYSCALL(getpgrp, 0);
}
