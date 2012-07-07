#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

int munmap(void *addr, size_t size)
{
    return INLINE_SYSCALL(mmap, 2, addr, size);
}
