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
 * Version of sbrk for no operating system.
 */

#include "config.h"
#include <_ansi.h>
#include <_syslist.h>
#include <sys/types.h>
#include <errno.h>
#undef errno
extern int errno;

extern char _end __attribute__ ((section (".bss"))); /* set by linker */

caddr_t 
_DEFUN (_sbrk, (incr),
        int incr)
{ 
   static char *heap_end; 
   char *prev_heap_end; 

   if (heap_end == 0) { 
      heap_end = &_end;
   } 
   prev_heap_end = heap_end; 
   heap_end += incr; 
   asm volatile ("mov\tet, %0" : : "r"(heap_end) );
   return (caddr_t) prev_heap_end; 
} 
