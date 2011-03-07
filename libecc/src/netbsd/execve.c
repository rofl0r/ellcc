#include <syscall.h>
#include <unistd.h>

int execve(const char* filename, char *const argv[], char *const envp[])
{
    return INLINE_SYSCALL(execve, 3, filename, argv, envp);
}
