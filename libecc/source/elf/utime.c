#include <sysdep.h>
#include <unistd.h>
#include <sys/types.h>

int utime(const char* filename, const struct utimbuf* times)
{
    return INLINE_SYSCALL(utime, 2, filename, times);
}
