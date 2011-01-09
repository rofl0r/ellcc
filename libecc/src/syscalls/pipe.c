#include <syscall.h>
#include <unistd.h>

int pipe(int pipefd[2])
{
    return INLINE_SYSCALL(pipe, 1, pipefd);
}
