#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

int madvise(void *addr, size_t size, int advice)
{
    return INLINE_SYSCALL(madvise, 3, addr, size, advice);
}
