#include <sysdep.h>
#include <unistd.h>

int fchown(int fd, uid_t owner, gid_t group)
{
    return INLINE_SYSCALL(fchown, 3, fd, owner, group);
}
