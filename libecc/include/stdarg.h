/*
 * stdarg.h
 *
 * Variable length argument lists.
 */

#ifndef _STDARG_H_
#define _STDARG_H_

typedef __builtin_va_list va_list;
#define va_start(ap, p)  __builtin_va_start(ap, p)
#define va_end(ap)  __builtin_va_end(ap)
#define va_arg(ap, type)  __builtin_va_arg(ap, type)
#define va_copy(dst ,src) __builtin_va_copy(dst , src)

#endif

