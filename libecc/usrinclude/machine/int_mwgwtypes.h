/*	$NetBSD: int_mwgwtypes.h,v 1.6 2008/04/28 20:23:24 martin Exp $	*/

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

#ifndef _MACHINE_INT_MWGWTYPES_H_
#define _MACHINE_INT_MWGWTYPES_H_

/* C99 7.18.1.2 Minimum-width integer types.
 * C99 7.18.1.3 Fastest minimum-width integer types.
 *
 * The standard requires that exact-width type be defined for 8-, 16-, 32-, and 
 * 64-bit types if they are implemented. Other exact width types are optional.
 * This implementation defines an exact-width types for every integer width
 * that is represented in the standard integer types.
 *
 * The standard also requires minimum-width types be defined for 8-, 16-, 32-,
 * and 64-bit widths regardless of whether there are corresponding exact-width
 * types. 
 *
 * To accommodate targets that are missing types that are exactly 8, 16, 32, or
 * 64 bits wide, this implementation takes an approach of cascading
 * redefintions, redefining __int_leastN_t to successively smaller exact-width
 * types. It is therefore important that the types are defined in order of
 * descending widths.
 *
 * We currently assume that the minimum-width types and the fastest
 * minimum-width types are the same. This is allowed by the standard, but is
 * suboptimal.
 *
 * In violation of the standard, some targets do not implement a type that is
 * wide enough to represent all of the required widths (8-, 16-, 32-, 64-bit).  
 * To accommodate these targets, a required minimum-width type is only
 * defined if there exists an exact-width type of equal or greater width.
 */

#ifdef __INT64_TYPE__
# define __int_least64_t int64_t
# define __uint_least64_t uint64_t
# define __int_least32_t int64_t
# define __uint_least32_t uint64_t
# define __int_least16_t int64_t
# define __uint_least16_t uint64_t
# define __int_least8_t int64_t
# define __uint_least8_t uint64_t
#endif /* __INT64_TYPE__ */

#ifdef __int_least64_t
typedef __int_least64_t int_least64_t;
typedef __uint_least64_t uint_least64_t;
typedef __int_least64_t int_fast64_t;
typedef __uint_least64_t uint_fast64_t;
#endif /* __int_least64_t */

#ifdef __INT56_TYPE__
typedef int56_t int_least56_t;
typedef uint56_t uint_least56_t;
typedef int56_t int_fast56_t;
typedef uint56_t uint_fast56_t;
# undef __int_least32_t
# undef __uint_least32_t
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least32_t int56_t
# define __uint_least32_t uint56_t
# define __int_least16_t int56_t
# define __uint_least16_t uint56_t
# define __int_least8_t int56_t
# define __uint_least8_t uint56_t
#endif /* __INT56_TYPE__ */

#ifdef __INT48_TYPE__
typedef int48_t int_least48_t;
typedef uint48_t uint_least48_t;
typedef int48_t int_fast48_t;
typedef uint48_t uint_fast48_t;
# undef __int_least32_t
# undef __uint_least32_t
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least32_t int48_t
# define __uint_least32_t uint48_t
# define __int_least16_t int48_t
# define __uint_least16_t uint48_t
# define __int_least8_t int48_t
# define __uint_least8_t uint48_t
#endif /* __INT48_TYPE__ */

#ifdef __INT40_TYPE__
typedef int40_t int_least40_t;
typedef uint40_t uint_least40_t;
typedef int40_t int_fast40_t;
typedef uint40_t uint_fast40_t;
# undef __int_least32_t
# undef __uint_least32_t
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least32_t int40_t
# define __uint_least32_t uint40_t
# define __int_least16_t int40_t
# define __uint_least16_t uint40_t
# define __int_least8_t int40_t
# define __uint_least8_t uint40_t
#endif /* __INT40_TYPE__ */

#ifdef __INT32_TYPE__
# undef __int_least32_t
# undef __uint_least32_t
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least32_t int32_t
# define __uint_least32_t uint32_t
# define __int_least16_t int32_t
# define __uint_least16_t uint32_t
# define __int_least8_t int32_t
# define __uint_least8_t uint32_t
#endif /* __INT32_TYPE__ */

#ifdef __int_least32_t
typedef __int_least32_t int_least32_t;
typedef __uint_least32_t uint_least32_t;
typedef __int_least32_t int_fast32_t;
typedef __uint_least32_t uint_fast32_t;
#endif /* __int_least32_t */

#ifdef __INT24_TYPE__
typedef int24_t int_least24_t;
typedef uint24_t uint_least24_t;
typedef int24_t int_fast24_t;
typedef uint24_t uint_fast24_t;
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least16_t int24_t
# define __uint_least16_t uint24_t
# define __int_least8_t int24_t
# define __uint_least8_t uint24_t
#endif /* __INT24_TYPE__ */

#ifdef __INT16_TYPE__
# undef __int_least16_t
# undef __uint_least16_t
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least16_t int16_t
# define __uint_least16_t uint16_t
# define __int_least8_t int16_t
# define __uint_least8_t uint16_t
#endif /* __INT16_TYPE__ */

#ifdef __int_least16_t
typedef __int_least16_t int_least16_t;
typedef __uint_least16_t uint_least16_t;
typedef __int_least16_t int_fast16_t;
typedef __uint_least16_t uint_fast16_t;
#endif /* __int_least16_t */

#ifdef __INT8_TYPE__
# undef __int_least8_t
# undef __uint_least8_t
# define __int_least8_t int8_t
# define __uint_least8_t uint8_t
#endif /* __INT8_TYPE__ */

#ifdef __int_least8_t
typedef __int_least8_t int_least8_t;
typedef __uint_least8_t uint_least8_t;
typedef __int_least8_t int_fast8_t;
typedef __uint_least8_t uint_fast8_t;
#endif /* __int_least8_t */

/* 7.18.1.5 Greatest-width integer types */

typedef __INTMAX_TYPE__  intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#endif /* !_MACHINE_INT_MWGWTYPES_H_ */
