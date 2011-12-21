#include <syscall.h>
#include <unistd.h>

int dup2(int oldfd, int newfd)
{
    return INLINE_SYSCALL(dup2, 2, oldfd, newfd);
}
