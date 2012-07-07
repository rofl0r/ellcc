#include <syscall.h>
#include <sys/types.h>
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence)
{
    return INLINE_SYSCALL(lseek, 3, fd, offset, whence);

#if RICH
#if defined(__ppc64__) || defined(__x86_64__)
    return INLINE_SYSCALL(lseek, 3, fd, offset, whence);
#elif defined(__mips__)
    loff_t loff;
    int result = INLINE_SYSCALL(_llseek, 5, fd, offset >> 32, offset & 0xFFFFFFFF, &loff, whence);
    if (result == 0) {
        return loff;
    }
    return result;
#else
    #error processor not defined
#endif
#endif
}
