#include <syscall.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>

struct kernel_dirent {
    ino_t d_fileno;
    off_t d_off;
    uint16_t d_reclen;
    char d_name[];
};

int getdents(int fd, char *buffer, size_t size)
{
    ssize_t bytes = INLINE_SYSCALL(getdents, 3, fd, buffer, size);
    if (bytes < 0) {
        return bytes;
    }
    union u {
        struct kernel_dirent kernel;
        struct dirent user;
    } *p = (union u *)buffer;
    while ((char *)p < buffer + bytes) {
        char type = *((char *)p + p->kernel.d_reclen - 1);
        memmove(p->user.d_name, p->kernel.d_name, strlen(p->kernel.d_name) + 1);
        p->user.d_type = type;
        p = (union u *)((char *)p + p->kernel.d_reclen);
    }
    return bytes;
}
