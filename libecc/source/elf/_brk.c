#include <sysdep.h>
#include <unistd.h>

extern char _end;

int _brk(void* addr)
{
    return INLINE_SYSCALL(brk, 1, addr);
}

void* _sbrk(intptr_t increment)
{
    static void* end = &_end;
    end += increment;
    if (!_brk(end)) {
        return NULL;
    }
    return end;
}
