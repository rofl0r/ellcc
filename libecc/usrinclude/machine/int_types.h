/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */

#ifndef	_MACHINE_INT_TYPES_H_
#define	_MACHINE_INT_TYPES_H_

#include <sys/cdefs.h>

/*
 * 7.18.1 Integer types
 */

/* 7.18.1.1 Exact-width integer types */

#ifdef __UINT8_TYPE__
  typedef __INT8_TYPE__ __int8_t;
  typedef __UINT8_TYPE__ __uint8_t;
#else
  #ifdef __INT8_TYPE__
  typedef signed __INT8_TYPE__ __int8_t;
  typedef unsigned __INT8_TYPE__ __uint8_t;
  #endif /* __INT8_TYPE__ */
#endif

#ifdef __UINT16_TYPE__
  typedef __INT16_TYPE__ __int16_t;
  typedef __UINT16_TYPE__ __uint16_t;
#else
  #ifdef __INT16_TYPE__
  typedef signed __INT16_TYPE__ __int16_t;
  typedef unsigned __INT16_TYPE__ __uint16_t;
  #endif /* __INT16_TYPE__ */
#endif

#ifdef __INT24_TYPE__
  typedef signed __INT24_TYPE__ __int24_t;
  typedef unsigned __INT24_TYPE__ __uint24_t;
#endif /* __INT24_TYPE__ */

#ifdef __UINT32_TYPE__
  typedef __INT32_TYPE__ __int32_t;
  typedef __UINT32_TYPE__ __uint32_t;
#else
  #ifdef __INT32_TYPE__
  typedef signed __INT32_TYPE__ __int32_t;
  typedef unsigned __INT32_TYPE__ __uint32_t;
  #endif /* __INT32_TYPE__ */
#endif

#ifdef __INT40_TYPE__
  typedef signed __INT40_TYPE__ __int40_t;
  typedef unsigned __INT40_TYPE__ __uint40_t;
#endif /* __INT40_TYPE__ */

#ifdef __INT48_TYPE__
  typedef signed __INT48_TYPE__ __int48_t;
  typedef unsigned __INT48_TYPE__ __uint48_t;
#endif /* __INT48_TYPE__ */

#ifdef __INT56_TYPE__
  typedef signed __INT56_TYPE__ __int56_t;
  typedef unsigned __INT56_TYPE__ __uint56_t;
#endif /* __INT56_TYPE__ */

#ifdef __UINT64_TYPE__
  typedef __INT64_TYPE__ __int64_t;
  typedef __UINT64_TYPE__ __uint64_t;
#else
  #ifdef __INT64_TYPE__
  typedef signed __INT64_TYPE__ __int64_t;
  typedef unsigned __INT64_TYPE__ __uint64_t;
  #endif /* __INT64_TYPE__ */
#endif

#define	__BIT_TYPES_DEFINED__

/* 7.18.1.4 Integer types capable of holding object pointers */

#define __stdint_join3(a,b,c) a ## b ## c

#define  __intn_t(n) __stdint_join3( __int, n, _t)
#define __uintn_t(n) __stdint_join3(__uint, n, _t)

#ifndef _INTPTR_T
#ifndef __intptr_t_defined
#ifdef __INTPTR_TYPE__
  typedef  __INTPTR_TYPE__  __intptr_t;
#else
  typedef  __intn_t(__INTPTR_WIDTH__)  __intptr_t;
#endif
#define __intptr_t_defined
#define _INTPTR_T
#endif
#endif

#ifndef _UINTPTR_T
#ifdef __UINTPTR_TYPE__
  typedef  __UINTPTR_TYPE__  __uintptr_t;
#else
  typedef __uintn_t(__INTPTR_WIDTH__) __uintptr_t;
#endif
#define _UINTPTR_T
#endif

#endif	/* !_MACHINE_INT_TYPES_H_ */
