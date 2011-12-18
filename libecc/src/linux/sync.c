#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

void sync(void)
{
    (void)INLINE_SYSCALL(sync, 0, 0);
}
