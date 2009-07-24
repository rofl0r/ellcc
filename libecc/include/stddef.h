/*
 * stddef.h
 *
 * Common definitions.
 */

#ifndef _STDDEF_H_
#define _STDDEF_H_

#include "_ansi.h"
_BEGIN_STD_C

#ifdef __LP64__
typedef unsigned long long int size_t;
typedef long long int ptrdiff_t;
#else
typedef unsigned int size_t;
typedef int ptrdiff_t;
#endif

typedef int wint_t;
typedef unsigned int wchar_t;

#define NULL 0
#define offsetof(type, field) ((size_t)&((type*)0)->field)

_END_STD_C

#endif
