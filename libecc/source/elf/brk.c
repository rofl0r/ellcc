#include <sysdep.h>
#include <unistd.h>
#include <errno.h>

static void* current;  // The current break pointer.

int _brk(void* addr)
{
    current = INLINE_SYSCALL(brk, 1, addr);
    if (current < addr) {
        __set_errno(ENOMEM);
        return -1;
    }

    return 0;
}

void* _sbrk(intptr_t increment)
{
    if (current == NULL && _brk(0)) {
        // Get the first brk pointer.
        return (void*)-1;
    }


    void* next = current;
    if (increment > 0) {
        if ((uintptr_t)next + (uintptr_t)increment < (uintptr_t)next) {
            // Overflow.
            return (void*)-1;
        }
    } else if ((uintptr_t)next < (uintptr_t)-increment) {
        // Underflow.
        return (void*)-1;
    }

    if (_brk(current + increment)) {
        return (void*)-1;
    }
    return next;
}
