/*	$NetBSD: int_limits.h,v 1.8 2008/04/28 20:23:24 martin Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MACHINE_INT_LIMITS_H_
#define _MACHINE_INT_LIMITS_H_

#include <machine/stdint_macros.h>

/* C99 7.18.2.1 Limits of exact-width integer types. 
 * C99 7.18.2.2 Limits of minimum-width integer types.
 * C99 7.18.2.3 Limits of fastest minimum-width integer types.
 *
 * The presence of limit macros are completely optional in C99.  This
 * implementation defines limits for all of the types (exact- and
 * minimum-width) that it defines above, using the limits of the minimum-width
 * type for any types that do not have exact-width representations.
 *
 * As in the type definitions, this section takes an approach of
 * successive-shrinking to determine which limits to use for the standard (8,
 * 16, 32, 64) bit widths when they don't have exact representations. It is
 * therefore important that the defintions be kept in order of decending
 * widths.
 *
 * Note that C++ should not check __STDC_LIMIT_MACROS here, contrary to the
 * claims of the C standard (see C++ 18.3.1p2, [cstdint.syn]).
 */

#ifdef __INT64_TYPE__
# define INT64_MAX           INT64_C( 9223372036854775807)
# define INT64_MIN         (-INT64_C( 9223372036854775807)-1)
# define UINT64_MAX         UINT64_C(18446744073709551615)
# define __INT_LEAST64_MIN   INT64_MIN
# define __INT_LEAST64_MAX   INT64_MAX
# define __UINT_LEAST64_MAX UINT64_MAX
# define __INT_LEAST32_MIN   INT64_MIN
# define __INT_LEAST32_MAX   INT64_MAX
# define __UINT_LEAST32_MAX UINT64_MAX
# define __INT_LEAST16_MIN   INT64_MIN
# define __INT_LEAST16_MAX   INT64_MAX
# define __UINT_LEAST16_MAX UINT64_MAX
# define __INT_LEAST8_MIN    INT64_MIN
# define __INT_LEAST8_MAX    INT64_MAX
# define __UINT_LEAST8_MAX  UINT64_MAX
#endif /* __INT64_TYPE__ */

#ifdef __INT_LEAST64_MIN
# define INT_LEAST64_MIN   __INT_LEAST64_MIN
# define INT_LEAST64_MAX   __INT_LEAST64_MAX
# define UINT_LEAST64_MAX __UINT_LEAST64_MAX
# define INT_FAST64_MIN    __INT_LEAST64_MIN
# define INT_FAST64_MAX    __INT_LEAST64_MAX
# define UINT_FAST64_MAX  __UINT_LEAST64_MAX
#endif /* __INT_LEAST64_MIN */

#ifdef __INT56_TYPE__
# define INT56_MAX           INT56_C(36028797018963967)
# define INT56_MIN         (-INT56_C(36028797018963967)-1)
# define UINT56_MAX         UINT56_C(72057594037927935)
# define INT_LEAST56_MIN     INT56_MIN
# define INT_LEAST56_MAX     INT56_MAX
# define UINT_LEAST56_MAX   UINT56_MAX
# define INT_FAST56_MIN      INT56_MIN
# define INT_FAST56_MAX      INT56_MAX
# define UINT_FAST56_MAX    UINT56_MAX
# undef __INT_LEAST32_MIN
# undef __INT_LEAST32_MAX
# undef __UINT_LEAST32_MAX
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST32_MIN   INT56_MIN
# define __INT_LEAST32_MAX   INT56_MAX
# define __UINT_LEAST32_MAX UINT56_MAX
# define __INT_LEAST16_MIN   INT56_MIN
# define __INT_LEAST16_MAX   INT56_MAX
# define __UINT_LEAST16_MAX UINT56_MAX
# define __INT_LEAST8_MIN    INT56_MIN
# define __INT_LEAST8_MAX    INT56_MAX
# define __UINT_LEAST8_MAX  UINT56_MAX
#endif /* __INT56_TYPE__ */

#ifdef __INT48_TYPE__
# define INT48_MAX           INT48_C(140737488355327)
# define INT48_MIN         (-INT48_C(140737488355327)-1)
# define UINT48_MAX         UINT48_C(281474976710655)
# define INT_LEAST48_MIN     INT48_MIN
# define INT_LEAST48_MAX     INT48_MAX
# define UINT_LEAST48_MAX   UINT48_MAX
# define INT_FAST48_MIN      INT48_MIN
# define INT_FAST48_MAX      INT48_MAX
# define UINT_FAST48_MAX    UINT48_MAX
# undef __INT_LEAST32_MIN
# undef __INT_LEAST32_MAX
# undef __UINT_LEAST32_MAX
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST32_MIN   INT48_MIN
# define __INT_LEAST32_MAX   INT48_MAX
# define __UINT_LEAST32_MAX UINT48_MAX
# define __INT_LEAST16_MIN   INT48_MIN
# define __INT_LEAST16_MAX   INT48_MAX
# define __UINT_LEAST16_MAX UINT48_MAX
# define __INT_LEAST8_MIN    INT48_MIN
# define __INT_LEAST8_MAX    INT48_MAX
# define __UINT_LEAST8_MAX  UINT48_MAX
#endif /* __INT48_TYPE__ */

#ifdef __INT40_TYPE__
# define INT40_MAX           INT40_C(549755813887)
# define INT40_MIN         (-INT40_C(549755813887)-1)
# define UINT40_MAX         UINT40_C(1099511627775)
# define INT_LEAST40_MIN     INT40_MIN
# define INT_LEAST40_MAX     INT40_MAX
# define UINT_LEAST40_MAX   UINT40_MAX
# define INT_FAST40_MIN      INT40_MIN
# define INT_FAST40_MAX      INT40_MAX
# define UINT_FAST40_MAX    UINT40_MAX
# undef __INT_LEAST32_MIN
# undef __INT_LEAST32_MAX
# undef __UINT_LEAST32_MAX
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST32_MIN   INT40_MIN
# define __INT_LEAST32_MAX   INT40_MAX
# define __UINT_LEAST32_MAX UINT40_MAX
# define __INT_LEAST16_MIN   INT40_MIN
# define __INT_LEAST16_MAX   INT40_MAX
# define __UINT_LEAST16_MAX UINT40_MAX
# define __INT_LEAST8_MIN    INT40_MIN
# define __INT_LEAST8_MAX    INT40_MAX
# define __UINT_LEAST8_MAX  UINT40_MAX
#endif /* __INT40_TYPE__ */

#ifdef __INT32_TYPE__
# define INT32_MAX           INT32_C(2147483647)
# define INT32_MIN         (-INT32_C(2147483647)-1)
# define UINT32_MAX         UINT32_C(4294967295)
# undef __INT_LEAST32_MIN
# undef __INT_LEAST32_MAX
# undef __UINT_LEAST32_MAX
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST32_MIN   INT32_MIN
# define __INT_LEAST32_MAX   INT32_MAX
# define __UINT_LEAST32_MAX UINT32_MAX
# define __INT_LEAST16_MIN   INT32_MIN
# define __INT_LEAST16_MAX   INT32_MAX
# define __UINT_LEAST16_MAX UINT32_MAX
# define __INT_LEAST8_MIN    INT32_MIN
# define __INT_LEAST8_MAX    INT32_MAX
# define __UINT_LEAST8_MAX  UINT32_MAX
#endif /* __INT32_TYPE__ */

#ifdef __INT_LEAST32_MIN
# define INT_LEAST32_MIN   __INT_LEAST32_MIN
# define INT_LEAST32_MAX   __INT_LEAST32_MAX
# define UINT_LEAST32_MAX __UINT_LEAST32_MAX
# define INT_FAST32_MIN    __INT_LEAST32_MIN
# define INT_FAST32_MAX    __INT_LEAST32_MAX
# define UINT_FAST32_MAX  __UINT_LEAST32_MAX
#endif /* __INT_LEAST32_MIN */

#ifdef __INT24_TYPE__
# define INT24_MAX           INT24_C(8388607)
# define INT24_MIN         (-INT24_C(8388607)-1)
# define UINT24_MAX         UINT24_C(16777215)
# define INT_LEAST24_MIN     INT24_MIN
# define INT_LEAST24_MAX     INT24_MAX
# define UINT_LEAST24_MAX   UINT24_MAX
# define INT_FAST24_MIN      INT24_MIN
# define INT_FAST24_MAX      INT24_MAX
# define UINT_FAST24_MAX    UINT24_MAX
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST16_MIN   INT24_MIN
# define __INT_LEAST16_MAX   INT24_MAX
# define __UINT_LEAST16_MAX UINT24_MAX
# define __INT_LEAST8_MIN    INT24_MIN
# define __INT_LEAST8_MAX    INT24_MAX
# define __UINT_LEAST8_MAX  UINT24_MAX
#endif /* __INT24_TYPE__ */

#ifdef __INT16_TYPE__
#define INT16_MAX            INT16_C(32767)
#define INT16_MIN          (-INT16_C(32767)-1)
#define UINT16_MAX          UINT16_C(65535)
# undef __INT_LEAST16_MIN
# undef __INT_LEAST16_MAX
# undef __UINT_LEAST16_MAX
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST16_MIN   INT16_MIN
# define __INT_LEAST16_MAX   INT16_MAX
# define __UINT_LEAST16_MAX UINT16_MAX
# define __INT_LEAST8_MIN    INT16_MIN
# define __INT_LEAST8_MAX    INT16_MAX
# define __UINT_LEAST8_MAX  UINT16_MAX
#endif /* __INT16_TYPE__ */

#ifdef __INT_LEAST16_MIN
# define INT_LEAST16_MIN   __INT_LEAST16_MIN
# define INT_LEAST16_MAX   __INT_LEAST16_MAX
# define UINT_LEAST16_MAX __UINT_LEAST16_MAX
# define INT_FAST16_MIN    __INT_LEAST16_MIN
# define INT_FAST16_MAX    __INT_LEAST16_MAX
# define UINT_FAST16_MAX  __UINT_LEAST16_MAX
#endif /* __INT_LEAST16_MIN */

#ifdef __INT8_TYPE__
# define INT8_MAX            INT8_C(127)
# define INT8_MIN          (-INT8_C(127)-1)
# define UINT8_MAX          UINT8_C(255)
# undef __INT_LEAST8_MIN
# undef __INT_LEAST8_MAX
# undef __UINT_LEAST8_MAX
# define __INT_LEAST8_MIN    INT8_MIN
# define __INT_LEAST8_MAX    INT8_MAX
# define __UINT_LEAST8_MAX  UINT8_MAX
#endif /* __INT8_TYPE__ */

#ifdef __INT_LEAST8_MIN
# define INT_LEAST8_MIN   __INT_LEAST8_MIN
# define INT_LEAST8_MAX   __INT_LEAST8_MAX
# define UINT_LEAST8_MAX __UINT_LEAST8_MAX
# define INT_FAST8_MIN    __INT_LEAST8_MIN
# define INT_FAST8_MAX    __INT_LEAST8_MAX
# define UINT_FAST8_MAX  __UINT_LEAST8_MAX
#endif /* __INT_LEAST8_MIN */

/* C99 7.18.2.4 Limits of integer types capable of holding object pointers. */
/* C99 7.18.3 Limits of other integer types. */

#if defined(__INTPTR_WIDTH__)
  #define  INTPTR_MIN  __INTN_MIN(__INTPTR_WIDTH__)
  #define  INTPTR_MAX  __INTN_MAX(__INTPTR_WIDTH__)
  #define UINTPTR_MAX __UINTN_MAX(__INTPTR_WIDTH__)
#elif defined(__INTPTR_MAX__)
  #define  INTPTR_MIN (-__INTPTR_MAX__-1)
  #define  INTPTR_MAX __INTPTR_MAX__
  #define UINTPTR_MAX __UINTPTR_MAX__
#endif

#if defined(__PTRDIFF_WIDTH__)
  #define PTRDIFF_MIN  __INTN_MIN(__PTRDIFF_WIDTH__)
  #define PTRDIFF_MAX  __INTN_MAX(__PTRDIFF_WIDTH__)
#elif defined(__PTRDIFF_MAX__)
  #define PTRDIFF_MIN  (-__PTRDIFF_MAX__-1)
  #define PTRDIFF_MAX  __PTRDIFF_MAX__
#endif

#if defined(__SIZE_WIDTH__)
  #define    SIZE_MAX __UINTN_MAX(__SIZE_WIDTH__)
#elif defined(__SIZE_MAX__)
  #define    SIZE_MAX __SIZE_MAX__
#endif

/* C99 7.18.2.5 Limits of greatest-width integer types. */
#if defined(__INTMAX_WIDTH__)
  #define INTMAX_MIN   __INTN_MIN(__INTMAX_WIDTH__)
  #define INTMAX_MAX   __INTN_MAX(__INTMAX_WIDTH__)
  #define UINTMAX_MAX __UINTN_MAX(__INTMAX_WIDTH__)
#elif defined(__INTMAX_MAX__)
  #define INTMAX_MIN   (-__INTMAX_MAX__-1)
  #define INTMAX_MAX   __INTMAX_MAX__
  #define UINTMAX_MAX  __UINTMAX_MAX__
#endif

/* C99 7.18.3 Limits of other integer types. */
#if defined(__SIG_ATOMIC_WIDTH__)
  #define SIG_ATOMIC_MIN __INTN_MIN(__SIG_ATOMIC_WIDTH__)
  #define SIG_ATOMIC_MAX __INTN_MAX(__SIG_ATOMIC_WIDTH__)
#elif defined(__SIG_ATOMIC_MAX__)
  #define SIG_ATOMIC_MIN __SIG_ATOMIC_MIN__
  #define SIG_ATOMIC_MAX __SIG_ATOMIC_MAX__
#endif

/* 7.18.4.2 Macros for greatest-width integer constants. */
#if defined(__INTMAX_WIDTH__)
  #define INTMAX_C(v)   __INTN_C(__INTMAX_WIDTH__, v)
  #define UINTMAX_C(v) __UINTN_C(__INTMAX_WIDTH__, v)
#elif defined(__INTMAX_C)
  #define INTMAX_C(v)   __INTMAX_C(v)
  #define UINTMAX_C(v) __UINTMAX_C(v)
#endif

#endif /* !_MACHINE_INT_LIMITS_H_ */
