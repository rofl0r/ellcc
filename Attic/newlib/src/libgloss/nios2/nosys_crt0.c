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

extern void __bss_start;
extern void _end;

extern int main (int, char **, char **);

void __fake_init () { /* Do nothing */ }
void _init () __attribute__ ((weak, alias ("__fake_init")));
          
 
void
__start_2 (void)
{
  _zero_bss (&__bss_start, 0, &_end - &__bss_start);

  _init ();

  exit (main (0, 0, 0));
}

