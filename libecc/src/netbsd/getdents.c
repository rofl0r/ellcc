#include <syscall.h>
#include <dirent.h>
#include <fcntl.h>

int getdents(int fd, char *buffer, size_t size)
{
    return 0;  // RICH: This has different meaning between NetBSD and Linux.
    return INLINE_SYSCALL(getdents, 3, fd, buffer, size);
}
