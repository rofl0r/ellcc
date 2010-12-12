#ifndef _ARM_LINUX_SYSCALL_H_
#define _ARM_LINUX_SYSCALL_H_

#include <asm/unistd.h>
#include <sys/errno.h>

/** Convert a system call name into the proper constant name.
 * @param name The system call name.
 * @return The name of the constant representing the system call.
 */
#define SYS_CONSTANT(name) (__NR_##name)

/** Check the result of a system call for an error.
 * @param result The result of the system call.
 * @return != 0 if the system call resulted in an error.
 *
 * Return values of -1 .. -4095 indicate error return values.
 */
#define IS_SYSCALL_ERROR(result) ((unsigned int)(result) >= 0xFFFFF001U)

/** Convert a system call result to a valid error number.
 * @param result A system call result indicating an error.
 * @return A valid errno value.
 */
#define SYSCALL_ERRNO(result) (-(result))

/** What's clobbered by a system call?
 */
#define SYSCALL_CLOBBERS "memory", "cc", "r4", "r14"

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
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\n\t"                                \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name))                           \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

/** A single argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 */
#define INLINE_SYSCALL_1(name, arg0)                                    \
    ({                                                                  \
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\t\n"                                \
                  "add r5, r0, %2\n\t"                                  \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "r" (arg0)                                         \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

/** A two argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 */
#define INLINE_SYSCALL_2(name, arg0, arg1)                              \
    ({                                                                  \
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\t\n"                                \
                  "add r5, r0, %2\n\t"                                  \
                  "add r6, r0, %3\n\t"                                  \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "r" (arg0), "r" (arg1)                             \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

/** A three argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 */
#define INLINE_SYSCALL_3(name, arg0, arg1, arg2)                        \
    ({                                                                  \
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\t\n"                                \
                  "add r5, r0, %2\n\t"                                  \
                  "add r6, r0, %3\n\t"                                  \
                  "add r6, r0, %4\n\t"                                  \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "r" (arg0), "r" (arg1), "r" (arg2)                 \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

/** A four argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 */
#define INLINE_SYSCALL_4(name, arg0, arg1, arg2, arg3)                  \
    ({                                                                  \
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\t\n"                                \
                  "add r5, r0, %2\n\t"                                  \
                  "add r6, r0, %3\n\t"                                  \
                  "add r6, r0, %4\n\t"                                  \
                  "add r7, r0, %5\n\t"                                  \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "r" (arg0), "r" (arg1), "r" (arg2)                 \
                     "r" (arg3)                                         \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
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
    unsigned int result;                                                \
    asm volatile ("addi r12, r0, %1\t\n"                                \
                  "add r5, r0, %2\n\t"                                  \
                  "add r6, r0, %3\n\t"                                  \
                  "add r6, r0, %4\n\t"                                  \
                  "add r7, r0, %5\n\t"                                  \
                  "add r8, r0, %6\n\t"                                  \
                  "brki r14, 0x8        # syscall " #name               \
                   : "={r3}" (result)                                   \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "r" (arg0), "r" (arg1), "r" (arg2)                 \
                     "r" (arg3), "r" (arg4)                             \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

#endif
