#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

void *mmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset)
{
    return (void *)INLINE_SYSCALL(mmap, 6, addr, size, prot, flags, fd, offset);
}
