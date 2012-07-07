#ifndef _X86_64_LINUX_SYSCALL_H_
#define _X86_64_LINUX_SYSCALL_H_

#include <asm/unistd.h>
#include <errno.h>

/** Convert a system call name into the proper constant name.
 * @param name The system call name.
 * @return The name if the constant representind the system call.
 *
 * System calls are defined in include/arm/linux/asm/unistd.h.
 */
#define SYS_CONSTANT(name) (__NR_##name)

/** Check the result of a system call for an error.
 * @param result The result of the system call.
 * @return 1 if the system call resulted in an error.
 *
 * Return values of -1 .. -4095 indicate error return values.
 */
#define IS_SYSCALL_ERROR(result) ((unsigned long int)(result) >= 0xFFFFFFFFFFFFF001UL)

/** Convert a system call result to a valid error number.
 * @param result A system call result indicating an error.
 * @return A valid errno value.
 */
#define SYSCALL_ERRNO(result) (-(result))

/** What's clobbered by a system call?
 */
#define SYSCALL_CLOBBERS "memory", "cc", "r11", "cx"

/** A system call.
 * @param name The system call name.
 * @param argcount The number of arguments.
 * @ return -1 if error or the result of the call.
 */
#define INLINE_SYSCALL(name, argcount, ...)                             \
   INLINE_SYSCALL_ ## argcount(name, __VA_ARGS__)

/** A no argument system call.
 * @param name The name of the system call.
 */
#define INLINE_SYSCALL_0(name, ...)                                     \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name)                              \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

#if defined(__clang__)
/** A single argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 */
#define INLINE_SYSCALL_1(name, arg0)                                    \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0)                                      \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A two argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 */
#define INLINE_SYSCALL_2(name, arg0, arg1)                              \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0), "{rsi}" (arg1)                      \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A three argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 */
#define INLINE_SYSCALL_3(name, arg0, arg1, arg2)                        \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0), "{rsi}" (arg1),                     \
                    "{rdx}" (arg2)                                      \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

#define INLINE_SYSCALL_3ili INLINE_SYSCALL_3

/** A four argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 */
#define INLINE_SYSCALL_4(name, arg0, arg1, arg2, arg3)                  \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0), "{rsi}" (arg1),                     \
                    "{rdx}" (arg2), "{r10}" (arg3)                      \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A five argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 * @param arg4 The fifth argument.
 */
#define INLINE_SYSCALL_5(name, arg0, arg1, arg2, arg3, arg4)            \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0), "{rsi}" (arg1),                     \
                    "{rdx}" (arg2), "{r10}" (arg3),                     \
                    "{r8}" (arg4)                                       \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A six argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 * @param arg4 The fifth argument.
 * @param arg5 The sixth argument.
 */
#define INLINE_SYSCALL_6(name, arg0, arg1, arg2, arg3, arg4, arg5)      \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "{rdi}" (arg0), "{rsi}" (arg1),                     \
                    "{rdx}" (arg2), "{r10}" (arg3),                     \
                    "{r8}" (arg4), "{r9}" (arg5)                        \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })
#else

// GCC compatable system call definitions.

/** A single argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 */
#define INLINE_SYSCALL_1(name, arg0)                                    \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0)                                          \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A two argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 */
#define INLINE_SYSCALL_2(name, arg0, arg1)                              \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0), "S" (arg1)                              \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A three argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 */
#define INLINE_SYSCALL_3(name, arg0, arg1, arg2)                        \
    ({                                                                  \
    unsigned long result;					        \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0), "S" (arg1),                             \
                    "d" (arg2)                                          \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

#define INLINE_SYSCALL_3ili INLINE_SYSCALL_3

/** A four argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 */
#define INLINE_SYSCALL_4(name, arg0, arg1, arg2, arg3)                  \
    ({                                                                  \
    unsigned long result;					        \
    register unsigned long r10 asm("r10") = arg3;                       \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0), "S" (arg1),                             \
                    "d" (arg2), "r" (r10)                               \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A five argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 * @param arg4 The fifth argument.
 */
#define INLINE_SYSCALL_5(name, arg0, arg1, arg2, arg3, arg4)            \
    ({                                                                  \
    unsigned long result;					        \
    register unsigned long r10 asm("r10") = arg3;                       \
    register unsigned long r8 asm("r8") = arg4;                         \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0), "S" (arg1),                             \
                    "d" (arg2), "r" (r10),                              \
                    "r" (r8)                                            \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })

/** A six argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 * @param arg4 The fifth argument.
 * @param arg5 The sixth argument.
 */
#define INLINE_SYSCALL_6(name, arg0, arg1, arg2, arg3, arg4, arg5)      \
    ({                                                                  \
    unsigned long result;					        \
    register unsigned long r10 asm("r10") = arg3;                       \
    register unsigned long r8 asm("r8") = arg4;                         \
    register unsigned long r9 asm("r9") = arg5;                         \
    asm volatile ("syscall      #" #name "\n\t"                         \
                  : "=a" (result)                                       \
                  : "0" SYS_CONSTANT(name),                             \
                    "D" (arg0), "S" (arg1),                             \
                    "d" (arg2), "r" (r10),                              \
                    "r" (r8), "r" (r9)                                  \
                  : SYSCALL_CLOBBERS);                                  \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    result;                                                             \
    })
#endif


#endif // _X86_64_LINUX_SYSCALL_H_
