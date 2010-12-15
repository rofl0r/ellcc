#ifndef _MIPS_LINUX_SYSDEP_H_
#define _MIPS_LINUX_SYSDEP_H_

#include <asm/unistd.h>
#include <sys/errno.h>

/** Convert a system call name into the proper constant name.
 * @param name The system call name.
 * @return The name if the constant representind the system call.
 *
 * System calls are defined in include/arm/linux/asm/unistd.h.
 */
#define SYS_CONSTANT(name) (__NR_##name)

/** Check the result of a system call for an error.
 * @param result The error return of the system call.
 * @return != 0 if the system call resulted in an error.
 */
#define IS_SYSCALL_ERROR(result) (result)

/** Convert a system call result to a valid error number.
 * @param result A system call result indicating an error.
 * @return A valid errno value.
 */
#define SYSCALL_ERRNO(result) (result)

/** A system call.
 * @param name The system call name.
 * @param argcount The number of arguments.
 * @ return -1 if error or the result of the call.
 */
#define INLINE_SYSCALL(name, argcount, ...)                             \
   INLINE_SYSCALL_ ## argcount(name, __VA_ARGS__)

#define SYSCALL_CLOBBERS "at", "v1", "t0", "t1", "t2", "t3", "t4", \
                         "t5", "t6", "t7", "t8", "t9", "memory"

/** A no argument system call.
 * @param name The name of the system call.
 */
#define INLINE_SYSCALL_0(name, ...)                                     \
    ({                                                                  \
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "syscall\n\t"                                         \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name))                           \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
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
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "syscall\n\t"                                         \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0)                                      \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
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
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "syscall\n\t"                                         \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1)                                      \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
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
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "syscall\n\t"                                         \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1),                                     \
                     "{a2}" (arg2)                                      \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
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
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "syscall\n\t"                                         \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1),                                     \
                     "{a2}" (arg2),                                     \
                     "{a3}" (arg3)                                      \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
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
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "subu sp, 32\n\t"                                     \
	          "sw %7, 16(sp)\n\t"                                   \
	          "syscall\n\t"                                         \
	          "addiu sp, 32\n\t"                                    \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1),                                     \
                     "{a2}" (arg2),                                     \
                     "{a3}" (arg3),                                     \
                     "r" (arg4)                                         \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
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
    alloca(4);          /* Force a frame pointer. */                    \
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "subu sp, 32\n\t"                                     \
	          "sw %7, 16(sp)\n\t"                                   \
	          "sw %8, 20(sp)\n\t"                                   \
	          "syscall\n\t"                                         \
	          "addiu sp, 32\n\t"                                    \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1),                                     \
                     "{a2}" (arg2),                                     \
                     "{a3}" (arg3),                                     \
                     "r" (arg4),                                        \
                     "r" (arg5)                                         \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

/** A seven argument system call.
 * @param name The name of the system call.
 * @param arg0 The first argument.
 * @param arg1 The second argument.
 * @param arg2 The third argument.
 * @param arg3 The fourth argument.
 * @param arg4 The fifth argument.
 * @param arg5 The sixth argument.
 * @param arg6 The seventh argument.
 */
#define INLINE_SYSCALL_7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6)\
    ({                                                                  \
    alloca(4);          /* Force a frame pointer. */                    \
    unsigned int result, err;					        \
    asm volatile (".set noreorder\n\t"                                  \
                  "li $v0, %2       # syscall " #name "\n\t"            \
	          "subu sp, 32\n\t"                                     \
	          "sw %7, 16(sp)\n\t"                                   \
	          "sw %8, 20(sp)\n\t"                                   \
	          "sw %9, 24(sp)\n\t"                                   \
	          "syscall\n\t"                                         \
	          "addiu sp, 32\n\t"                                    \
	          ".set reorder"                                        \
                   : "={v0}" (result), "={a3}" (err)                    \
                   : "i" (SYS_CONSTANT(name)),                          \
                     "{a0}" (arg0),                                     \
                     "{a1}" (arg1),                                     \
                     "{a2}" (arg2),                                     \
                     "{a3}" (arg3),                                     \
                     "r" (arg4),                                        \
                     "r" (arg5),                                        \
                     "r" (arg6)                                         \
                   : SYSCALL_CLOBBERS);                                 \
    if (IS_SYSCALL_ERROR(err)) {                                        \
        __set_errno(SYSCALL_ERRNO(err));                                \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

#endif // _MIPS_LINUX_SYSDEP_H_
