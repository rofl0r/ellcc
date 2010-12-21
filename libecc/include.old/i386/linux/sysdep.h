/* Copyright (C) 1992,1993,1995-2000,2002-2006,2007
   	Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper, <drepper@gnu.org>, August 1995.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _I386_LINUX_SYSDEP_H_
#define _I386_LINUX_SYSDEP_H_

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
 * @param result The result of the system call.
 * @return 1 if the system call resulted in an error.
 *
 * Return values of -1 .. -4095 indicate error return values.
 */
#define IS_SYSCALL_ERROR(result) ((unsigned int)(result) >= 0xFFFFF001U)

/** Convert a system call result to a valid error number.
 * @param result A system call result indicating an error.
 * @return A valid errno value.
 */
#define SYSCALL_ERRNO(result) (-(result))

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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name)                                   \
                  : "memory", "cc");                                    \
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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name), "b" (arg0)                       \
                  : "memory", "cc");                                    \
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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name), "b" (arg0), "c" (arg1)           \
                  : "memory", "cc");                                    \
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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name), "b" (arg0), "c" (arg1),          \
                                       "d" (arg2)                       \
                  : "memory", "cc");                                    \
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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name), "b" (arg0), "c" (arg1),          \
                                       "d" (arg2), "S" (arg3)           \
                  : "memory", "cc");                                    \
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
    unsigned int result;					        \
    asm volatile ("movl %1, %%eax\n\t"                                  \
                  "int $0x80    # syscall " #name "\n\t"                \
                  : "=a" (result)                                       \
                  : "i" (__NR_##name), "b" (arg0), "c" (arg1),          \
                                       "d" (arg2), "S" (arg3),          \
                                       "D" (arg4)                       \
                  : "memory", "cc");                                    \
    if (IS_SYSCALL_ERROR(result)) {                                     \
        __set_errno(SYSCALL_ERRNO(result));                             \
        result = -1;                                                    \
    }                                                                   \
    (int) result;                                                       \
    })

#endif
