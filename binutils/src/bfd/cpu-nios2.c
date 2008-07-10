/* bfd back-end for Altera Nios II support

   Copyright (C) 2003
   by Nigel Gray (ngray@altera.com).
 
This file is part of BFD, the Binary File Descriptor library.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"

static const bfd_arch_info_type *nios2_compatible
  (const bfd_arch_info_type *, const bfd_arch_info_type *);

/* The default routine tests bits_per_word, which is wrong on mips as
   mips word size doesn't correlate with reloc size.  */

static const bfd_arch_info_type *
nios2_compatible (const bfd_arch_info_type *a, const bfd_arch_info_type *b)
{
  if (a->arch != b->arch)
    return NULL;

  /* Machine compatibility is checked in
     _bfd_mips_elf_merge_private_bfd_data.  */

  return a;
}

#define N(BITS_WORD, BITS_ADDR, NUMBER, PRINT, DEFAULT, NEXT)		\
  {							\
    BITS_WORD, /*  bits in a word */			\
    BITS_ADDR, /* bits in an address */			\
    8,	/* 8 bits in a byte */				\
    bfd_arch_nios2,					\
    NUMBER,						\
    "nios2",						\
    PRINT,						\
    3,							\
    DEFAULT,						\
    nios2_compatible,					\
    bfd_default_scan,					\
    NEXT,						\
  }

#define NN(index) (&arch_info_struct[(index) + 1])

static const bfd_arch_info_type arch_info_struct[] =
{
  N (32, 32, bfd_mach_nios2, "nios2", FALSE, 0),
};

/* There is only one architecture - but we give the default a machine number of 0
   so the linker can distinguish it */
const bfd_arch_info_type bfd_nios2_arch =
N (32, 32, 0, "nios2", TRUE, &arch_info_struct[0]);
