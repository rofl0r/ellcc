#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

int getgroups(int size, gid_t list[])
{
    return INLINE_SYSCALL(getgroups, 2, size, list);
}
