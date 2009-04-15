/* Copyright 2005, 2007 Shaun Jackman
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */
/* doc in dprintf.c */

#include <reent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include "local.h"

int _vdprintf_r(struct _reent *ptr, int fd, const char *format, va_list ap)
{
  char *p;
  char buf[512];
  size_t n = sizeof buf;

  _REENT_SMALL_CHECK_INIT (ptr);
  p = _vasnprintf_r (ptr, buf, &n, format, ap);
  if (!p)
    return -1;
  n = _write_r (ptr, fd, p, n);
  if (p != buf)
    _free_r (ptr, p);
  return n;
}

#ifndef _REENT_ONLY

int vdprintf(int fd, const char *format, va_list ap)
{
  return _vdprintf_r (_REENT, fd, format, ap);
}

#endif /* ! _REENT_ONLY */
