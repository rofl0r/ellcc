#include <syscall.h>
#include <unistd.h>

int chown(const char* path, uid_t owner, gid_t group)
{
    return INLINE_SYSCALL(chown, 3, path, owner, group);
}
