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

#ifndef __WARNING_H__
#define __WARNING_H__

#define link_warning(symbol, msg)


#if 0


#ifdef HAVE_GNU_LD
# ifdef HAVE_ELF

/* We want the .gnu.warning.SYMBOL section to be unallocated.  */
#  ifdef HAVE_ASM_PREVIOUS_DIRECTIVE
#   define __make_section_unallocated(section_string)   \
  asm(".section " section_string "\n\t .previous");
#  elif defined (HAVE_ASM_POPSECTION_DIRECTIVE)
#   define __make_section_unallocated(section_string)   \
  asm(".pushsection " section_string "\n\t .popsection");
#  else
#   define __make_section_unallocated(section_string)
#  endif

#  ifdef HAVE_SECTION_ATTRIBUTES
#   define link_warning(symbol, msg)                     \
  __make_section_unallocated (".gnu.warning." #symbol)  \
  static const char __evoke_link_warning_##symbol[]     \
    __attribute__ ((section (".gnu.warning." #symbol))) = msg;
#  else
#   define link_warning(symbol, msg)
#  endif

#else /* !ELF */

#  define link_warning(symbol, msg)             \
  asm(".stabs \"" msg "\",30,0,0,0\n"   \
      ".stabs \"" __SYMBOL_PREFIX #symbol "\",1,0,0,0\n");
# endif
#else /* !GNULD */
/* We will never be heard; they will all die horribly.  */
# define link_warning(symbol, msg)
#endif

#endif /* if 0 */

/* A canned warning for sysdeps/stub functions.  */
#define stub_warning(name) \
  link_warning (name, \
                "warning: " #name " is not implemented and will always fail")

#endif /* __WARNING_H__ */
