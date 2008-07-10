/* Altera New Jersey ELF support for BFD

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

/* This file holds definitions specific to the Altera New Jersey ELF ABI.  Note
   that most of this is not actually implemented by BFD.  */

#ifndef _ELF_NIOS2_H
#define _ELF_NIOS2_H

#include "elf/reloc-macros.h"

/* the order of these numbers must match the order in
   the elf_nios2_howto_table_rel table for the lookup
   function to work properly */

START_RELOC_NUMBERS (elf_nios2_reloc_type)
  RELOC_NUMBER (R_NIOS2_NONE,	  0)
  RELOC_NUMBER (R_NIOS2_S16,	  1)
  RELOC_NUMBER (R_NIOS2_U16,	  2)
  RELOC_NUMBER (R_NIOS2_PCREL16, 3)
  RELOC_NUMBER (R_NIOS2_CALL26,  4)
  RELOC_NUMBER (R_NIOS2_IMM5,	  5)
  RELOC_NUMBER (R_NIOS2_CACHE_OPX, 6)
  RELOC_NUMBER (R_NIOS2_IMM6,	  7)
  RELOC_NUMBER (R_NIOS2_IMM8,	  8)
  RELOC_NUMBER (R_NIOS2_HI16,	  9)
  RELOC_NUMBER (R_NIOS2_LO16,	  10)
  RELOC_NUMBER (R_NIOS2_HIADJ16, 11)
  RELOC_NUMBER (R_NIOS2_BFD_RELOC_32, 12)
  RELOC_NUMBER (R_NIOS2_BFD_RELOC_16, 13)
  RELOC_NUMBER (R_NIOS2_BFD_RELOC_8, 14)
  RELOC_NUMBER (R_NIOS2_GPREL, 15)
  RELOC_NUMBER (R_NIOS2_GNU_VTINHERIT, 16)
  RELOC_NUMBER (R_NIOS2_GNU_VTENTRY, 17)
  RELOC_NUMBER (R_NIOS2_UJMP, 18)
  RELOC_NUMBER (R_NIOS2_CJMP, 19)
  RELOC_NUMBER (R_NIOS2_CALLR, 20)
  RELOC_NUMBER (R_NIOS2_ALIGN, 21)
  RELOC_NUMBER (R_NIOS2_ILLEGAL, 22)
END_RELOC_NUMBERS (R_NIOS2_maxext)

/* Processor specific section flags */

/* This is used to mark gp-relative sections */
#define SHF_NIOS2_GPREL	0x10000000

#endif //_ELF_NIOS2_H
