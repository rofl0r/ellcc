#include <syscall.h>
#include <unistd.h>

int lchown(const char *path, uid_t owner, gid_t group)
{
    return INLINE_SYSCALL(lchown, 3, path, owner, group);
}
