/*	$NetBSD: int_const.h,v 1.3 2010/05/29 17:33:57 tnozaki Exp $	*/

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

#ifndef _MACHINE_INT_CONST_H_
#define _MACHINE_INT_CONST_H_

/* C99 7.18.4 Macros for minimum-width integer constants.
 *
 * The standard requires that integer constant macros be defined for all the
 * minimum-width types defined above. As 8-, 16-, 32-, and 64-bit minimum-width
 * types are required, the corresponding integer constant macros are defined 
 * here. This implementation also defines minimum-width types for every other
 * integer width that the target implements, so corresponding macros are 
 * defined below, too.
 *
 * These macros are defined using the same successive-shrinking approach as
 * the type definitions above. It is likewise important that macros are defined
 * in order of decending width.
 *
 * Note that C++ should not check __STDC_CONSTANT_MACROS here, contrary to the
 * claims of the C standard (see C++ 18.3.1p2, [cstdint.syn]).
 */

#define __int_c_join(a, b) a ## b
#define __int_c(v, suffix) __int_c_join(v, suffix)
#define __uint_c(v, suffix) __int_c_join(v##U, suffix)

#ifdef __INT64_TYPE__
# ifdef __INT64_C_SUFFIX__
#  define __int64_c_suffix __INT64_C_SUFFIX__
#  define __int32_c_suffix __INT64_C_SUFFIX__
#  define __int16_c_suffix __INT64_C_SUFFIX__
#  define  __int8_c_suffix __INT64_C_SUFFIX__
# else
#  undef __int64_c_suffix
#  undef __int32_c_suffix
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT64_C_SUFFIX__ */
#endif /* __INT64_TYPE__ */

#ifdef __int_least64_t
# ifdef __int64_c_suffix
#  define INT64_C(v) __int_c(v, __int64_c_suffix)
#  define UINT64_C(v) __uint_c(v, __int64_c_suffix)
# else
#  define INT64_C(v) v ## LL
#  define UINT64_C(v) v ## ULL
# endif /* __int64_c_suffix */
#endif /* __int_least64_t */


#ifdef __INT56_TYPE__
# ifdef __INT56_C_SUFFIX__
#  define INT56_C(v) __int_c(v, __INT56_C_SUFFIX__)
#  define UINT56_C(v) __uint_c(v, __INT56_C_SUFFIX__)
#  define __int32_c_suffix __INT56_C_SUFFIX__
#  define __int16_c_suffix __INT56_C_SUFFIX__
#  define __int8_c_suffix  __INT56_C_SUFFIX__
# else
#  define INT56_C(v) v ## LL
#  define UINT56_C(v) v ## ULL
#  undef __int32_c_suffix
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT56_C_SUFFIX__ */
#endif /* __INT56_TYPE__ */

#ifdef __INT48_TYPE__
# ifdef __INT48_C_SUFFIX__
#  define INT48_C(v) __int_c(v, __INT48_C_SUFFIX__)
#  define UINT48_C(v) __uint_c(v, __INT48_C_SUFFIX__)
#  define __int32_c_suffix __INT48_C_SUFFIX__
#  define __int16_c_suffix __INT48_C_SUFFIX__
#  define __int8_c_suffix  __INT48_C_SUFFIX__
# else
#  define INT48_C(v) v ## LL
#  define UINT48_C(v) v ## ULL
#  undef __int32_c_suffix
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT48_C_SUFFIX__ */
#endif /* __INT48_TYPE__ */

#ifdef __INT40_TYPE__
# ifdef __INT40_C_SUFFIX__
#  define INT40_C(v) __int_c(v, __INT40_C_SUFFIX__)
#  define UINT40_C(v) __uint_c(v, __INT40_C_SUFFIX__)
#  define __int32_c_suffix __INT40_C_SUFFIX__
#  define __int16_c_suffix __INT40_C_SUFFIX__
#  define __int8_c_suffix  __INT40_C_SUFFIX__
# else
#  define INT40_C(v) v ## LL
#  define UINT40_C(v) v ## ULL
#  undef __int32_c_suffix
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT40_C_SUFFIX__ */
#endif /* __INT40_TYPE__ */

#ifdef __INT32_TYPE__
# ifdef __INT32_C_SUFFIX__
#  define __int32_c_suffix __INT32_C_SUFFIX__
#  define __int16_c_suffix __INT32_C_SUFFIX__
#  define __int8_c_suffix  __INT32_C_SUFFIX__
#else
#  undef __int32_c_suffix
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT32_C_SUFFIX__ */
#endif /* __INT32_TYPE__ */

#ifdef __int_least32_t
# ifdef __int32_c_suffix
#  define INT32_C(v) __int_c(v, __int32_c_suffix)
#  define UINT32_C(v) __uint_c(v, __int32_c_suffix)
# else
#  define INT32_C(v) v
#  define UINT32_C(v) v ## U
# endif /* __int32_c_suffix */
#endif /* __int_least32_t */


#ifdef __INT24_TYPE__
# ifdef __INT24_C_SUFFIX__
#  define INT24_C(v) __int_c(v, __INT24_C_SUFFIX__)
#  define UINT24_C(v) __uint_c(v, __INT24_C_SUFFIX__)
#  define __int16_c_suffix __INT24_C_SUFFIX__
#  define __int8_c_suffix  __INT24_C_SUFFIX__
# else
#  define INT24_C(v) v
#  define UINT24_C(v) v ## U
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT24_C_SUFFIX__ */
#endif /* __INT24_TYPE__ */

#ifdef __INT16_TYPE__
# ifdef __INT16_C_SUFFIX__
#  define __int16_c_suffix __INT16_C_SUFFIX__
#  define __int8_c_suffix  __INT16_C_SUFFIX__
#else
#  undef __int16_c_suffix
#  undef  __int8_c_suffix
# endif /* __INT16_C_SUFFIX__ */
#endif /* __INT16_TYPE__ */

#ifdef __int_least16_t
# ifdef __int16_c_suffix
#  define INT16_C(v) __int_c(v, __int16_c_suffix)
#  define UINT16_C(v) __uint_c(v, __int16_c_suffix)
# else
#  define INT16_C(v) v
#  define UINT16_C(v) v ## U
# endif /* __int16_c_suffix */
#endif /* __int_least16_t */

#ifdef __INT8_TYPE__
# ifdef __INT8_C_SUFFIX__
#  define __int8_c_suffix __INT8_C_SUFFIX__
#else
#  undef  __int8_c_suffix
# endif /* __INT8_C_SUFFIX__ */
#endif /* __INT8_TYPE__ */

#ifdef __int_least8_t
# ifdef __int8_c_suffix
#  define INT8_C(v) __int_c(v, __int8_c_suffix)
#  define UINT8_C(v) __uint_c(v, __int8_c_suffix)
# else
#  define INT8_C(v) v
#  define UINT8_C(v) v ## U
# endif /* __int8_c_suffix */
#endif /* __int_least8_t */

#endif /* !_MACHINE_INT_CONST_H_ */
