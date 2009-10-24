/* 
   Copyright (C) 2003
   by Jonah Graham (jgraham@altera.com).

   This file is part of Newlib.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*
 * Stub version of gettimeofday.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#undef errno
extern int errno;
#include "warning.h"

struct timeval;
struct timezone;

int
_DEFUN (_gettimeofday, (ptimeval, ptimezone),
        struct timeval  *ptimeval  _AND
        struct timezone *ptimezone)
{
  errno = ENOSYS;
  return -1;
}

stub_warning(_gettimeofday)
