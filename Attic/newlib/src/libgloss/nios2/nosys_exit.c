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

extern void _do_dtors (void);
void __fake_fini () { /* Do nothing */ }
void _fini () __attribute__ ((weak, alias ("__fake_fini")));

void
_exit (int exit_code)
{
  _fini ();
    
  /* ??? May want a return to germs (or other) here? */
  __asm__ (
	   "mov\tr2, %0\n"
	   "0:\n"
	   "\tbr\t0b" 
	   :  
	   : "r" (exit_code));
}
