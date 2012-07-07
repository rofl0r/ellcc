/*	$NetBSD: wchar_limits.h,v 1.2 2008/04/28 20:23:24 martin Exp $	*/

/*-
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
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

#ifndef _MACHINE_WCHAR_LIMITS_H_
#define _MACHINE_WCHAR_LIMITS_H_

#include <machine/stdint_macros.h>

/*
 * 7.18.3 Limits of other integer types
 */

/* limits of wint_t */
#if defined(__WINT_WIDTH__)
  #if defined(__WINT_UNSIGNED__)
    #define WINT_MIN       __UINTN_C(__WINT_WIDTH__, 0)
    #define WINT_MAX       __UINTN_MAX(__WINT_WIDTH__)
  #else
    #define WINT_MIN       __INTN_MIN(__WINT_WIDTH__)
    #define WINT_MAX       __INTN_MAX(__WINT_WIDTH__)
  #endif
#else
  #define WINT_MIN       __WINT_MIN__
  #define WINT_MAX       __WINT_MAX__
#endif

/* limits of wchar_t */
#if !defined(WCHAR_MAX)
  #define WCHAR_MAX __WCHAR_MAX__
#endif
#if defined(__WCHAR_WIDTH__)
  #if !defined(WCHAR_MIN)
    #if __WCHAR_MAX__ == __INTN_MAX(__WCHAR_WIDTH__)
      #define WCHAR_MIN __INTN_MIN(__WCHAR_WIDTH__)
    #else
      #define WCHAR_MIN __UINTN_C(__WCHAR_WIDTH__, 0)
    #endif
  #endif
#else
  #define WCHAR_MIN __WCHAR_MIN__
#endif

#endif /* !_MACHINE_WCHAR_LIMITS_H_ */
