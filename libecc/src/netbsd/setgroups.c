#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int setgroups(int size, const gid_t *list)
{
    return INLINE_SYSCALL(setgroups, 2, size, list);
}
