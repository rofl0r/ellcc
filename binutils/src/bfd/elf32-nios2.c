/* New Jersey-specific support for 32-bit ELF

   Copyright (C) 2005
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

/* This file handles Altera New Jersey ELF targets  */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "bfdlink.h"
#include "genlink.h"
#include "elf-bfd.h"
#include "elf/nios2.h"
#include "opcode/nios2.h"

/* use RELA relocations*/
#ifndef USE_RELA
#define USE_RELA
#endif

#ifdef USE_REL
#undef USE_REL
#endif

/* Function prototypes */

static reloc_howto_type *nios2_elf32_bfd_reloc_type_lookup
  (bfd *, bfd_reloc_code_real_type);

static reloc_howto_type *nios2_elf32_bfd_reloc_name_lookup
 (bfd *abfd ATTRIBUTE_UNUSED, const char *r_name);

static bfd_boolean nios2_elf32_relax_section
  (bfd *, asection *, struct bfd_link_info *, bfd_boolean *);

#if RICH
static bfd_boolean nios2_elf32_relax_delete_bytes
  (bfd *, asection *, bfd_vma, int);
#endif

static reloc_howto_type *nios2_elf32_rtype_to_howto
  (unsigned int r_type, bfd_boolean rela_p);

static void nios2_elf32_info_to_howto
  (bfd * abfd, arelent * cache_ptr, Elf_Internal_Rela * dst);

static bfd_boolean nios2_elf32_relocate_section
  (bfd * output_bfd, struct bfd_link_info * info, bfd * input_bfd,
   asection * input_section, bfd_byte * contents,
   Elf_Internal_Rela * relocs, Elf_Internal_Sym * local_syms,
   asection ** local_sections);

static reloc_howto_type *lookup_howto   (unsigned int rtype);

static bfd_reloc_status_type nios2_elf_final_gp
  (bfd *, asymbol *, bfd_boolean, char **, bfd_vma *, struct bfd_link_info *);

static bfd_boolean nios2_elf_assign_gp (bfd *, bfd_vma *, struct bfd_link_info *);

static bfd_reloc_status_type nios2_elf32_ignore_reloc
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_hi16_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_lo16_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_hiadj16_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_pcrel16_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_call26_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_gprel_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_ujmp_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_cjmp_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_callr_relocate
  (bfd *, arelent *, asymbol *, void *, asection *, bfd *, char **);

static bfd_reloc_status_type nios2_elf32_do_hi16_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_lo16_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_hiadj16_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_pcrel16_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_call26_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_gprel_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_ujmp_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_cjmp_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);

static bfd_reloc_status_type nios2_elf32_do_callr_relocate
  (bfd *, reloc_howto_type *, asection *,
	 bfd_byte *, bfd_vma, bfd_vma, bfd_vma);


static void nios2_elf32_post_process_headers
  (bfd *, struct bfd_link_info *);

static bfd_boolean nios2_elf32_section_from_shdr
  (bfd *, Elf_Internal_Shdr *, const char *name, int shindex);

static bfd_boolean nios2_elf32_section_flags
  (flagword *flags, const Elf_Internal_Shdr *hdr);

static bfd_boolean nios2_elf32_fake_sections
  (bfd *, Elf_Internal_Shdr *, asection *);



static bfd_boolean nios2_elf32_check_relocs
  (bfd *, struct bfd_link_info *, asection *,
	 const Elf_Internal_Rela *);

static asection *nios2_elf32_gc_mark_hook   (asection * sec,
						   struct bfd_link_info *
						   info,
						   Elf_Internal_Rela * rel,
						   struct elf_link_hash_entry
						   * h,
						   Elf_Internal_Sym * sym);


/* target vector */
extern const bfd_target bfd_elf32_littlenios2_vec;
extern const bfd_target bfd_elf32_bignios2_vec;

/* The relocation table used for SHT_REL sections.  */

static reloc_howto_type elf_nios2_howto_table_rel[] = {
  /* No relocation.  */
  HOWTO (R_NIOS2_NONE,		/* type */
	 0,			/* rightshift */
	 0,			/* size (0 = byte, 1 = short, 2 = long) */
	 0,			/* bitsize */
	 FALSE,			/* pc_relative */
	 0,			/* bitpos */
	 complain_overflow_dont,	/* complain_on_overflow */
	 bfd_elf_generic_reloc,	/* special_function */
	 "R_NIOS2_NONE",	/* name */
	 FALSE,			/* partial_inplace */
	 0,			/* src_mask */
	 0,			/* dst_mask */
	 FALSE),		/* pcrel_offset */

  /* 16-bit signed immediate relocation */
  HOWTO (R_NIOS2_S16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_signed,	/* complain on overflow */
	 bfd_elf_generic_reloc,	/* special function */
	 "R_NIOS2_S16",		/* name */
	 FALSE,			/* partial_inplace */
	 0x003fffc0,		/* src_mask */
	 0x003fffc0,		/* dest_mask */
	 FALSE),		/* pcrel_offset */

  /* 16-bit unsigned immediate relocation */
  HOWTO (R_NIOS2_U16,		/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_unsigned,	/* complain on overflow */
	 bfd_elf_generic_reloc,	/* special function */
	 "R_NIOS2_U16",		/* name */
	 FALSE,			/* partial_inplace */
	 0x003fffc0,		/* src_mask */
	 0x003fffc0,		/* dest_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_NIOS2_PCREL16,	/* type */
	 0,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 16,			/* bitsize */
	 TRUE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_signed,	/* complain on overflow */
	 nios2_elf32_pcrel16_relocate,	/* special function */
	 "R_NIOS2_PCREL16",	/* name */
	 FALSE,			/* partial_inplace */
	 0x003fffc0,		/* src_mask */
	 0x003fffc0,		/* dest_mask */
	 TRUE),			/* pcrel_offset */

  HOWTO (R_NIOS2_CALL26,	/* type */
	 2,			/* rightshift */
	 2,			/* size (0 = byte, 1 = short, 2 = long) */
	 26,			/* bitsize */
	 FALSE,			/* pc_relative */
	 6,			/* bitpos */
	 complain_overflow_dont,	/* complain on overflow */
	 nios2_elf32_call26_relocate,	/* special function */
	 "R_NIOS2_CALL26",	/* name */
	 FALSE,			/* partial_inplace */
	 0xffffffc0,		/* src_mask */
	 0xffffffc0,		/* dst_mask */
	 FALSE),		/* pcrel_offset */

  HOWTO (R_NIOS2_IMM5,
	 0,
	 2,
	 5,
	 FALSE,
	 6,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_IMM5",
	 FALSE,
	 0x000007c0,
	 0x000007c0,
	 FALSE),

  HOWTO (R_NIOS2_CACHE_OPX,
	 0,
	 2,
	 5,
	 FALSE,
	 22,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_CACHE_OPX",
	 FALSE,
	 0x07c00000,
	 0x07c00000,
	 FALSE),

  HOWTO (R_NIOS2_IMM6,
	 0,
	 2,
	 6,
	 FALSE,
	 6,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_IMM6",
	 FALSE,
	 0x00000fc0,
	 0x00000fc0,
	 FALSE),

  HOWTO (R_NIOS2_IMM8,
	 0,
	 2,
	 8,
	 FALSE,
	 6,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_IMM8",
	 FALSE,
	 0x00003fc0,
	 0x00003fc0,
	 FALSE),

  HOWTO (R_NIOS2_HI16,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_hi16_relocate,
	 "R_NIOS2_HI16",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_LO16,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_lo16_relocate,
	 "R_NIOS2_LO16",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_HIADJ16,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_hiadj16_relocate,
	 "R_NIOS2_HIADJ16",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_BFD_RELOC_32,
	 0,
	 2,			/* long */
	 32,
	 FALSE,
	 0,
	 complain_overflow_dont,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_BFD_RELOC32",
	 FALSE,
	 0xffffffff,
	 0xffffffff,
	 FALSE),

  HOWTO (R_NIOS2_BFD_RELOC_16,
	 0,
	 1,			/* short */
	 16,
	 FALSE,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_BFD_RELOC16",
	 FALSE,
	 0x0000ffff,
	 0x0000ffff,
	 FALSE),

  HOWTO (R_NIOS2_BFD_RELOC_8,
	 0,
	 0,			/* byte */
	 8,
	 FALSE,
	 0,
	 complain_overflow_bitfield,
	 bfd_elf_generic_reloc,
	 "R_NIOS2_BFD_RELOC8",
	 FALSE,
	 0x000000ff,
	 0x000000ff,
	 FALSE),

  HOWTO (R_NIOS2_GPREL,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_gprel_relocate,
	 "R_NIOS2_GPREL",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_GNU_VTINHERIT,
	 0,
	 2,			/* short */
	 0,
	 FALSE,
	 0,
	 complain_overflow_dont,
	 NULL,
	 "R_NIOS2_GNU_VTINHERIT",
	 FALSE,
	 0,
	 0,
	 FALSE),

  HOWTO (R_NIOS2_GNU_VTENTRY,
	 0,
	 2,			/* byte */
	 0,
	 FALSE,
	 0,
	 complain_overflow_dont,
	 _bfd_elf_rel_vtable_reloc_fn,
	 "R_NIOS2_GNU_VTENTRY",
	 FALSE,
	 0,
	 0,
	 FALSE),

  HOWTO (R_NIOS2_UJMP,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_ujmp_relocate,
	 "R_NIOS2_UJMP",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_CJMP,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_cjmp_relocate,
	 "R_NIOS2_CJMP",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_CALLR,
	 0,
	 2,
	 32,
	 FALSE,
	 6,
	 complain_overflow_dont,
	 nios2_elf32_callr_relocate,
	 "R_NIOS2_CALLR",
	 FALSE,
	 0x003fffc0,
	 0x003fffc0,
	 FALSE),

  HOWTO (R_NIOS2_ALIGN,
	 0,
	 2,
	 0,
	 FALSE,
	 0,
	 complain_overflow_dont,
	 nios2_elf32_ignore_reloc,
	 "R_NIOS2_ALIGN",
	 FALSE,
	 0,
	 0,
	 TRUE),

/* add other relocations here */
};

static unsigned char elf_code_to_howto_index[R_NIOS2_ILLEGAL + 1];

static reloc_howto_type *
lookup_howto (unsigned int rtype)
{
  static int initialized = 0;
  int i;
  int howto_tbl_size = (int) (sizeof (elf_nios2_howto_table_rel)
			      / sizeof (elf_nios2_howto_table_rel[0]));

  if (!initialized)
    {
      initialized = 1;
      memset (elf_code_to_howto_index, 0xff,
	      sizeof (elf_code_to_howto_index));
      for (i = 0; i < howto_tbl_size; i++)
	elf_code_to_howto_index[elf_nios2_howto_table_rel[i].type] = i;
    }

  BFD_ASSERT (rtype <= R_NIOS2_ILLEGAL);
  i = elf_code_to_howto_index[rtype];
  if (i >= howto_tbl_size)
    return 0;
  return elf_nios2_howto_table_rel + i;
}

/*
   map for converting BFD reloc types to New Jersey
   reloc types
 */
struct elf_reloc_map
{
  bfd_reloc_code_real_type bfd_val;
  enum elf_nios2_reloc_type elf_val;
};

static const struct elf_reloc_map nios2_reloc_map[] = {
  {BFD_RELOC_NIOS2_S16, R_NIOS2_S16},
  {BFD_RELOC_NIOS2_U16, R_NIOS2_U16},
  {BFD_RELOC_16_PCREL, R_NIOS2_PCREL16},
  {BFD_RELOC_NIOS2_CALL26, R_NIOS2_CALL26},
  {BFD_RELOC_NIOS2_IMM5, R_NIOS2_IMM5},
  {BFD_RELOC_NIOS2_CACHE_OPX, R_NIOS2_CACHE_OPX},
  {BFD_RELOC_NIOS2_IMM6, R_NIOS2_IMM6},
  {BFD_RELOC_NIOS2_IMM8, R_NIOS2_IMM8},
  {BFD_RELOC_NIOS2_HI16, R_NIOS2_HI16},
  {BFD_RELOC_NIOS2_LO16, R_NIOS2_LO16},
  {BFD_RELOC_NIOS2_HIADJ16, R_NIOS2_HIADJ16},
  {BFD_RELOC_32, R_NIOS2_BFD_RELOC_32},
  {BFD_RELOC_16, R_NIOS2_BFD_RELOC_16},
  {BFD_RELOC_8, R_NIOS2_BFD_RELOC_8},
  {BFD_RELOC_NIOS2_GPREL, R_NIOS2_GPREL},
  {BFD_RELOC_VTABLE_INHERIT, R_NIOS2_GNU_VTINHERIT},
  {BFD_RELOC_VTABLE_ENTRY, R_NIOS2_GNU_VTENTRY},
  {BFD_RELOC_NIOS2_UJMP, R_NIOS2_UJMP},
  {BFD_RELOC_NIOS2_CJMP, R_NIOS2_CJMP},
  {BFD_RELOC_NIOS2_CALLR, R_NIOS2_CALLR},
  {BFD_RELOC_NIOS2_ALIGN, R_NIOS2_ALIGN},
};

/* Given a BFD reloc type, return a howto structure.  */

static reloc_howto_type *
nios2_elf32_bfd_reloc_type_lookup (bfd * abfd ATTRIBUTE_UNUSED,
				   bfd_reloc_code_real_type code)
{
  int i;
  for (i = 0;
       i < (int) (sizeof (nios2_reloc_map) / sizeof (struct elf_reloc_map));
       ++i)
    {
      if (nios2_reloc_map[i].bfd_val == code)
	return &elf_nios2_howto_table_rel[(int) nios2_reloc_map[i].elf_val];
    }

  return NULL;
}

static reloc_howto_type *
nios2_elf32_bfd_reloc_name_lookup (bfd *abfd ATTRIBUTE_UNUSED,
			     const char *r_name)
{
  unsigned int i;

  for (i = 0;
       i < (sizeof (elf_nios2_howto_table_rel)
	    / sizeof (elf_nios2_howto_table_rel[0]));
       i++)
    if (elf_nios2_howto_table_rel[i].name != NULL
	&& strcasecmp (elf_nios2_howto_table_rel[i].name, r_name) == 0)
      return &elf_nios2_howto_table_rel[i];

  return NULL;
}

/* Helper function for nios2_elf32_info_to_howto */

static reloc_howto_type *
nios2_elf32_rtype_to_howto (unsigned int r_type,
			    bfd_boolean rela_p ATTRIBUTE_UNUSED)
{
  BFD_ASSERT (r_type < R_NIOS2_ILLEGAL);
  return &elf_nios2_howto_table_rel[r_type];
}

/* Given a ELF32 relocation, fill in a arelent structure */

static void
nios2_elf32_info_to_howto (bfd * abfd ATTRIBUTE_UNUSED, arelent * cache_ptr,
			   Elf_Internal_Rela * dst)
{
  unsigned int r_type;

  r_type = ELF32_R_TYPE (dst->r_info);
  cache_ptr->howto = nios2_elf32_rtype_to_howto (r_type, FALSE);

  // FIXME - do we need to do anything else here???
}

/* The assembler has output long jmp/call sequences for all calls
 * and pc-relative branches that it cannot guarantee are within
 * range, so the linker must attempt to "relax" these sequences to
 * short branches and calls if it can. We avoid having to re-relax by
 * replacing redundant instructions with nops instead of deleting them.
 *
 *
 **/
static bfd_boolean
nios2_elf32_relax_section (bfd * abfd,
			   asection * sec,
			   struct bfd_link_info *link_info, bfd_boolean * again)
{
  Elf_Internal_Shdr *symtab_hdr;
  Elf_Internal_Rela *internal_relocs;
  Elf_Internal_Rela *irel, *irelend;
  bfd_byte *contents = NULL;
  Elf_Internal_Sym *isymbuf = NULL;

#define OP_MATCH_NOP 0x0001883a

  /* Assume nothing changes.  */
  *again = FALSE;

  /* We don't have to do anything for a relocatable link, if
     this section does not have relocs, or if this is not a
     code section.  */
  if (link_info->relocatable
      || (sec->flags & SEC_RELOC) == 0
      || sec->reloc_count == 0 || (sec->flags & SEC_CODE) == 0)
    return TRUE;

  /* If this is the first time we have been called for this section,
     initialize the cooked size.  */
  if (sec->size == 0)
    sec->size = sec->rawsize;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;

  /* Get a copy of the native relocations.  */
  internal_relocs = (_bfd_elf_link_read_relocs
		     (abfd, sec, (void *) NULL, (Elf_Internal_Rela *) NULL,
		      link_info->keep_memory));
  if (internal_relocs == NULL)
    goto error_return;

  /* Walk through them looking for relaxing opportunities.  */
  irelend = internal_relocs + sec->reloc_count;
  for (irel = internal_relocs; irel < irelend; irel++)
    {
      bfd_vma symval;

      /* If this isn't something that can be relaxed, then ignore
         this reloc.  */
      if (ELF32_R_TYPE (irel->r_info) != (int) R_NIOS2_UJMP
	  && ELF32_R_TYPE (irel->r_info) != (int) R_NIOS2_CJMP
	  && ELF32_R_TYPE (irel->r_info) != (int) R_NIOS2_CALLR)
	{
	  continue;
	}

      /* Get the section contents if we haven't done so already.  */
      if (contents == NULL)
	{
	  /* Get cached copy if it exists.  */
	  if (elf_section_data (sec)->this_hdr.contents != NULL)
	    contents = elf_section_data (sec)->this_hdr.contents;
	  else
	    {
	      /* Go get them off disk.  */
	      contents = (bfd_byte *) bfd_malloc (sec->rawsize);
	      if (contents == NULL)
		goto error_return;

	      if (!bfd_get_section_contents (abfd, sec, contents,
					     (file_ptr) 0, sec->rawsize))
		goto error_return;
	    }
	}

      /* Read this BFD's local symbols if we haven't done so already.  */
      if (isymbuf == NULL && symtab_hdr->sh_info != 0)
	{
	  isymbuf = (Elf_Internal_Sym *) symtab_hdr->contents;
	  if (isymbuf == NULL)
	    isymbuf = bfd_elf_get_elf_syms (abfd, symtab_hdr,
					    symtab_hdr->sh_info, 0,
					    NULL, NULL, NULL);
	  if (isymbuf == NULL)
	    goto error_return;
	}

      /* Get the value of the symbol referred to by the reloc.  */
      if (ELF32_R_SYM (irel->r_info) < symtab_hdr->sh_info)
	{
	  /* A local symbol.  */
	  Elf_Internal_Sym *isym;
	  asection *sym_sec;

	  isym = isymbuf + ELF32_R_SYM (irel->r_info);
	  if (isym->st_shndx == SHN_UNDEF)
	    sym_sec = bfd_und_section_ptr;
	  else if (isym->st_shndx == SHN_ABS)
	    sym_sec = bfd_abs_section_ptr;
	  else if (isym->st_shndx == SHN_COMMON)
	    sym_sec = bfd_com_section_ptr;
	  else
	    sym_sec = bfd_section_from_elf_index (abfd, isym->st_shndx);
	  symval = (isym->st_value
		    + sym_sec->output_section->vma + sym_sec->output_offset);
	}
      else
	{
	  unsigned long indx;
	  struct elf_link_hash_entry *h;

	  /* An external symbol.  */
	  indx = ELF32_R_SYM (irel->r_info) - symtab_hdr->sh_info;
	  h = elf_sym_hashes (abfd)[indx];
	  BFD_ASSERT (h != NULL);
	  if (h->root.type != bfd_link_hash_defined
	      && h->root.type != bfd_link_hash_defweak)
	    {
	      /* This appears to be a reference to an undefined
	         symbol.  Just ignore it--it will be caught by the
	         regular reloc processing.  */
	      continue;
	    }

	  symval = (h->root.u.def.value
		    + h->root.u.def.section->output_section->vma
		    + h->root.u.def.section->output_offset);
	}

      /* For simplicity of coding, we are going to modify the section
         contents, the section relocs, and the BFD symbol table.  We
         must tell the rest of the code not to free up this
         information.  It would be possible to instead create a table
         of changes which have to be made, as is done in coff-mips.c;
         that would be more work, but would require less memory when
         the linker is run.  */

      /* try to turn :
       *      movhi at, %hi(symbol)
       *      movui at, %lo(symbol)
       *      callr at
       * into:
       *      call symbol
       */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_NIOS2_CALLR)
	{
	  bfd_vma targ_addr = symval + irel->r_addend;
	  bfd_vma curr_addr = (sec->output_section->vma + sec->output_offset);
	  bfd_vma targ_page, curr_page;
	  targ_page = targ_addr & 0xf0000000;
	  curr_page = curr_addr & 0xf0000000;

	  if (targ_page == curr_page)
	    {
	      /* change the opcode to a call */
	      bfd_put_32 (abfd, OP_MATCH_CALL, contents + irel->r_offset);
	      /* Note that we've changed the relocs, section contents, etc.  */
	      elf_section_data (sec)->relocs = internal_relocs;
	      elf_section_data (sec)->this_hdr.contents = contents;
	      symtab_hdr->contents = (unsigned char *) isymbuf;

	      /* Fix the relocation's type.  */
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
					   R_NIOS2_CALL26);

	      /* replace next two instructions with nops */
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 4);
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 8);
	    }
	}

      /* try to turn :
       *    movhi at, %hi(symbol)
       *    movui at, %lo(symbol)
       *    jmp at
       * into:
       *    br symbol
       */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_NIOS2_UJMP)
	{
	  bfd_vma pcrel_offset;
	  Elf_Internal_Rela *irelalign = NULL;
	  Elf_Internal_Rela *irela = elf_section_data (sec)->relocs;
	  Elf_Internal_Rela *irelend = irel + sec->reloc_count;

	  for (; irela < irelend; irela++)
	    {
	      if (ELF32_R_TYPE (irela->r_info) == (int) R_NIOS2_ALIGN
		  && irela->r_offset > irel->r_offset + 4
		  && 8 < (1 << irela->r_addend))
		{
		  irelalign = irela;
		  break;
		}
	    }

	  /* calculate the pcrelative offset from current location */
	  pcrel_offset = symval;
	  pcrel_offset -= (sec->output_section->vma + sec->output_offset);
	  pcrel_offset += irel->r_addend;

	  /* we need to compute the pcrel_offset from the next instruction */
	  pcrel_offset -= (irel->r_offset + 4);

	  /* does this value fit in 16 bits  */
	  if ((irelalign == NULL && (long) pcrel_offset <= 0x8004
	       && (long) pcrel_offset >= -0x8000) || (irelalign != NULL
						      && (long) pcrel_offset
						      <= 0x7ffc
						      && (long) pcrel_offset
						      >= -0x8000))
	    {
	      /* change the opcode to an unconditional branch */
	      bfd_put_32 (abfd, OP_MATCH_BR, contents + irel->r_offset);
	      /* Note that we've changed the relocs, section contents, etc.  */
	      elf_section_data (sec)->relocs = internal_relocs;
	      elf_section_data (sec)->this_hdr.contents = contents;
	      symtab_hdr->contents = (unsigned char *) isymbuf;

	      /* Fix the relocation's type.  */
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
					   R_NIOS2_PCREL16);

	      /* replace next two instructions with nops */
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 4);
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 8);
	    }
	}

      /* try to turn :
       *   b{cond} a, b skip
       *   movhi at, %hi(symbol)
       *   movui at, %lo(symbol)
       *   jmp at
       * skip:
       *   ...
       * into:
       *   br{opp_cond} a, b, symbol
       */
      if (ELF32_R_TYPE (irel->r_info) == (int) R_NIOS2_CJMP)
	{
	  bfd_vma pcrel_offset;
	  Elf_Internal_Rela *irelalign = NULL;
	  Elf_Internal_Rela *irela = elf_section_data (sec)->relocs;
	  Elf_Internal_Rela *irelend = irel + sec->reloc_count;

	  for (; irela < irelend; irela++)
	    {
	      if (ELF32_R_TYPE (irela->r_info) == (int) R_NIOS2_ALIGN
		  && irela->r_offset > irel->r_offset + 4
		  && 8 < (1 << irela->r_addend))
		{
		  irelalign = irela;
		  break;
		}
	    }

	  /* calculate the pcrelative offset from current location */
	  pcrel_offset = symval;
	  pcrel_offset -= (sec->output_section->vma + sec->output_offset);
	  pcrel_offset += irel->r_addend;

	  /* we need to compute the pcrel_offset from this instruction
	   * ie the movhi */
	  pcrel_offset -= (irel->r_offset);

	  /* does this value fit in 16 bits */
	  if ((irelalign == NULL && (long) pcrel_offset <= 0x8008
	       && (long) pcrel_offset >= -0x8000) || (irelalign != NULL
						      && (long) pcrel_offset
						      <= 0x7ffc
						      && (long) pcrel_offset
						      >= -0x8000))
	    {
	      unsigned long opcode, op_a, op_b;
	      /* get the conditional branch opcode */
	      opcode = bfd_get_32 (abfd, contents + irel->r_offset - 4);
	      /* reverse the condition */
	      switch (opcode & OP_MASK_OP)
		{
		case OP_MATCH_BEQ:
		  opcode = (opcode & ~OP_MASK_OP) | OP_MATCH_BNE;
		  break;
		case OP_MATCH_BNE:
		  opcode = (opcode & ~OP_MASK_OP) | OP_MATCH_BEQ;
		  break;
		case OP_MATCH_BGE:
		case OP_MATCH_BGEU:
		case OP_MATCH_BLT:
		case OP_MATCH_BLTU:
		  /* swap the operands */
		  op_a = (opcode & OP_MASK_RRT) << 5;
		  op_b = (opcode & OP_MASK_RRS) >> 5;
		  opcode =
		    (opcode & ~(OP_MASK_RRS | OP_MASK_RRT)) | op_a | op_b;
		  break;
		default:
		  fprintf (stderr,
			   "relaxation error - expecting conditional branch, aborting\n");
		  abort ();
		  break;
		}

	      /* we must set the branch target to zero so that the skip over the jmp doesn't get
	       * added to the jmp */
	      opcode = opcode & (~OP_MASK_IMM16);

	      /* change the opcode to the reversed conditional branch */
	      bfd_put_32 (abfd, opcode, contents + irel->r_offset - 4);
	      /* Note that we've changed the relocs, section contents, etc.  */
	      elf_section_data (sec)->relocs = internal_relocs;
	      elf_section_data (sec)->this_hdr.contents = contents;
	      symtab_hdr->contents = (unsigned char *) isymbuf;

	      /* Fix the relocation's type.  */
	      irel->r_info = ELF32_R_INFO (ELF32_R_SYM (irel->r_info),
					   R_NIOS2_PCREL16);

	      /* this relocation's offset has also been reduced by 4 bytes */
	      irel->r_offset -= 4;

	      /* replace next two instructions with nops */
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 4);
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 8);
	      bfd_put_32 (abfd, OP_MATCH_NOP, contents + irel->r_offset + 12);
	    }
	}

      /* otherwise, leave alone */
    }

  if (isymbuf != NULL && symtab_hdr->contents != (unsigned char *) isymbuf)
    {
      if (!link_info->keep_memory)
	free (isymbuf);
      else
	{
	  /* Cache the symbols for elf_link_input_bfd.  */
	  symtab_hdr->contents = (unsigned char *) isymbuf;
	}
    }

  if (contents != NULL
      && elf_section_data (sec)->this_hdr.contents != contents)
    {
      if (!link_info->keep_memory)
	free (contents);
      else
	{
	  /* Cache the section contents for elf_link_input_bfd.  */
	  elf_section_data (sec)->this_hdr.contents = contents;
	}
    }

  if (internal_relocs != NULL
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);


  return TRUE;

error_return:
  if (isymbuf != NULL && symtab_hdr->contents != (unsigned char *) isymbuf)
    free (isymbuf);
  if (contents != NULL
      && elf_section_data (sec)->this_hdr.contents != contents)
    free (contents);
  if (internal_relocs != NULL
      && elf_section_data (sec)->relocs != internal_relocs)
    free (internal_relocs);

  return FALSE;
}

#if RICH
/* Delete some bytes from a section while relaxing.
 * Copied from mn10200 port */

static bfd_boolean
nios2_elf32_relax_delete_bytes (bfd * abfd,
				asection * sec, bfd_vma addr, int count)
{
  Elf_Internal_Shdr *symtab_hdr;
  unsigned int sec_shndx;
  bfd_byte *contents;
  Elf_Internal_Rela *irel, *irelend;
  Elf_Internal_Rela *irelalign;
  bfd_vma toaddr;
  Elf_Internal_Sym *isym;
  Elf_Internal_Sym *isymend;
  struct elf_link_hash_entry **sym_hashes;
  struct elf_link_hash_entry **end_hashes;
  unsigned int symcount;
  asection *asec;

  sec_shndx = _bfd_elf_section_from_bfd_section (abfd, sec);

  contents = elf_section_data (sec)->this_hdr.contents;

  /* The deletion must stop at the next ALIGN reloc for an aligment
     power larger than the number of bytes we are deleting.  */

  irelalign = NULL;
  /* +1 because we need to readjust symbols at end of section */
  toaddr = sec->size + 1;

  irel = elf_section_data (sec)->relocs;
  irelend = irel + sec->reloc_count;

  for (; irel < irelend; irel++)
    {
      if (ELF32_R_TYPE (irel->r_info) == (int) R_NIOS2_ALIGN
	  && irel->r_offset > addr && count < (1 << irel->r_addend))
	{
	  irelalign = irel;
	  /* +1 because we need to readjust symbols at end of section */
	  toaddr = irel->r_offset + 1;
	  break;
	}
    }


  /* Actually delete the bytes.  */
  memmove (contents + addr, contents + addr + count,
	   (size_t) ((toaddr - 1) - addr - count));

  if (irelalign == NULL)
    sec->size -= count;
  else
    {
      int i;

#define NOP_OPCODE  (0x0001883a)

      BFD_ASSERT ((count & 3) == 0);
      for (i = 0; i < count; i += 4)
	bfd_put_32 (abfd, (bfd_vma) NOP_OPCODE,
		    contents + (toaddr - 1) - count + i);
    }

  /* get the symbol table */
  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  isym = (Elf_Internal_Sym *) symtab_hdr->contents;

  /* Adjust all the reloc offsets in this section.  */
  for (irel = elf_section_data (sec)->relocs; irel < irelend; irel++)
    {
      /* Get the new reloc address.  */
      if ((irel->r_offset > addr && irel->r_offset < toaddr))
	irel->r_offset -= count;
    }

  /* Adjust relocations against targets in this section whose positions
   * have moved as a result of the relaxation */

  for (asec = abfd->sections; asec; asec = asec->next)
    {
      irelend = elf_section_data (asec)->relocs + asec->reloc_count;
      for (irel = elf_section_data (asec)->relocs; irel < irelend; irel++)
	{
	  Elf_Internal_Sym *sym;
	  /* if the symbol which this reloc is against doesn't change
	   * we need to change the reloc addend */

	  sym = isym + ELF32_R_SYM (irel->r_info);
	  if (sym->st_shndx == sec_shndx
	      && !(sym->st_value > addr && sym->st_value < toaddr)
	      && sym->st_value + irel->r_addend > addr
	      && sym->st_value + irel->r_addend < toaddr)
	    {
	      irel->r_addend -= count;
	    }

	}
    }

  /* Adjust the local symbols defined in this section.  */
  for (isymend = isym + symtab_hdr->sh_info; isym < isymend; isym++)
    {
      if (isym->st_shndx == sec_shndx
	  && isym->st_value > addr && isym->st_value < toaddr)
	isym->st_value -= count;


    }

  /* Now adjust the global symbols defined in this section.  */
  symcount = (symtab_hdr->sh_size / sizeof (Elf32_External_Sym)
	      - symtab_hdr->sh_info);
  sym_hashes = elf_sym_hashes (abfd);
  end_hashes = sym_hashes + symcount;
  for (; sym_hashes < end_hashes; sym_hashes++)
    {
      struct elf_link_hash_entry *sym_hash = *sym_hashes;
      if ((sym_hash->root.type == bfd_link_hash_defined
	   || sym_hash->root.type == bfd_link_hash_defweak)
	  && sym_hash->root.u.def.section == sec
	  && sym_hash->root.u.def.value > addr
	  && sym_hash->root.u.def.value < toaddr)
	{
	  sym_hash->root.u.def.value -= count;
	}
    }

  return TRUE;
}
#endif

struct bfd_link_info *nios2_link_info = NULL;

#if RICH
void
_bfd_set_link_info (struct bfd_link_info *info)
{
  nios2_link_info = info;
}

bfd_boolean linker_force_make_executable = FALSE;

void
_bfd_set_force_make_executable (bfd_boolean force)
{
  linker_force_make_executable = force;
}
#endif

/* Set the GP value for OUTPUT_BFD.  Returns FALSE if this is a
   dangerous relocation.  */

static bfd_boolean
nios2_elf_assign_gp (bfd *output_bfd, bfd_vma *pgp, struct bfd_link_info *info)
{
  bfd_boolean gp_found;
  struct bfd_hash_entry *h;
  struct bfd_link_hash_entry *lh;

  /* If we've already figured out what GP will be, just return it. */
  *pgp = _bfd_get_gp_value (output_bfd);
  if (*pgp)
    return TRUE;

  h = bfd_hash_lookup (&info->hash->table, "_gp", FALSE, FALSE);
  lh = (struct bfd_link_hash_entry *) h;
lookup:
  if (lh)
    {
      switch (lh->type)
	{
	case bfd_link_hash_undefined:
	case bfd_link_hash_undefweak:
	case bfd_link_hash_common:
	  gp_found = FALSE;
	  break;
	case bfd_link_hash_defined:
	case bfd_link_hash_defweak:
	  gp_found = TRUE;
	  *pgp = lh->u.def.value;
	  break;
	case bfd_link_hash_indirect:
	case bfd_link_hash_warning:
	  lh = lh->u.i.link;
	  /* @@FIXME  ignoring warning for now */
	  goto lookup;
	case bfd_link_hash_new:
	default:
	  abort ();
	}
    }
  else
    gp_found = FALSE;

  if (!gp_found)
    {
      /* Only get the error once. */
      *pgp = 4;
      _bfd_set_gp_value (output_bfd, *pgp);
      return FALSE;
    }

  _bfd_set_gp_value (output_bfd, *pgp);

  return TRUE;
}

/* We have to figure out the gp value, so that we can adjust the
   symbol value correctly.  We look up the symbol _gp in the output
   BFD.  If we can't find it, we're stuck.  We cache it in the ELF
   target data.  We don't need to adjust the symbol value for an
   external symbol if we are producing relocatable output.  */

static bfd_reloc_status_type
nios2_elf_final_gp (bfd *output_bfd, asymbol *symbol, bfd_boolean relocatable, 
                    char **error_message, bfd_vma *pgp, struct bfd_link_info *info)
{
  if (bfd_is_und_section (symbol->section) && !relocatable)
    {
      *pgp = 0;
      return bfd_reloc_undefined;
    }

  *pgp = _bfd_get_gp_value (output_bfd);
  if (*pgp == 0 && (!relocatable || (symbol->flags & BSF_SECTION_SYM) != 0))
    {
      /* if this is called without link_info, then
         we cannot be doing a final link */
      if (info == NULL)
	relocatable = TRUE;

      if (relocatable)
	{
	  /* Make up a value.  */
	  *pgp = symbol->section->output_section->vma + 0x4000;
	  _bfd_set_gp_value (output_bfd, *pgp);
	}
      else if (!nios2_elf_assign_gp (output_bfd, pgp, info))
	{
	  *error_message =
	    (char *)
	    _("global pointer relative relocation when _gp not defined");
	  return bfd_reloc_dangerous;
	}
    }

  return bfd_reloc_ok;
}


/* Relocations that require special handling */

/* This is for relocations used only when relaxing to ensure
 * changes in size of section don't screw up .align */
static bfd_reloc_status_type
nios2_elf32_ignore_reloc (bfd *abfd ATTRIBUTE_UNUSED, arelent *reloc_entry,
     asymbol *symbol ATTRIBUTE_UNUSED, void *data ATTRIBUTE_UNUSED,
     asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
  if (output_bfd != NULL)
    reloc_entry->address += input_section->output_offset;
  return bfd_reloc_ok;
}

static bfd_reloc_status_type
nios2_elf32_hi16_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol, void *data,
     asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_hi16_relocate (abfd, reloc_entry->howto,
				       input_section,
				       data, reloc_entry->address,
				       (symbol->value
					+ symbol->section->output_section->vma
					+ symbol->section->output_offset),
				       reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_lo16_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
/* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_lo16_relocate (abfd, reloc_entry->howto,
				       input_section,
				       data, reloc_entry->address,
				       (symbol->value
					+ symbol->section->output_section->vma
					+ symbol->section->output_offset),
				       reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_hiadj16_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, 
     char **error_message ATTRIBUTE_UNUSED)
{
/* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_hiadj16_relocate (abfd, reloc_entry->howto,
					  input_section,
					  data, reloc_entry->address,
					  (symbol->value
					   +
					   symbol->section->output_section->
					   vma +
					   symbol->section->output_offset),
					  reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_pcrel16_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, 
     char **error_message ATTRIBUTE_UNUSED)
{
/* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_pcrel16_relocate (abfd, reloc_entry->howto,
					  input_section,
					  data, reloc_entry->address,
					  (symbol->value
					   +
					   symbol->section->output_section->
					   vma +
					   symbol->section->output_offset),
					  reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_call26_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **error_message ATTRIBUTE_UNUSED)
{
/* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_call26_relocate (abfd, reloc_entry->howto,
					 input_section,
					 data, reloc_entry->address,
					 (symbol->value
					  +
					  symbol->section->output_section->
					  vma +
					  symbol->section->output_offset),
					 reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_gprel_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **msg)
{
  bfd_vma relocation;
  bfd_vma gp;
  bfd_reloc_status_type r;


/* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  relocation = symbol->value
    + symbol->section->output_section->vma + symbol->section->output_offset;

  if ((r =
       nios2_elf_final_gp (abfd, symbol, FALSE, msg, &gp, nios2_link_info)) == bfd_reloc_ok)
    {
      relocation = relocation + reloc_entry->addend - gp;
      reloc_entry->addend = 0;
      if ((signed) relocation < -32768 || (signed) relocation > 32767)
	{
	  *msg = _("global pointer relative address out of range");
	  r = bfd_reloc_outofrange;
	}
      else
	{
	  r = nios2_elf32_do_gprel_relocate (abfd, reloc_entry->howto,
					     input_section,
					     data, reloc_entry->address,
					     relocation, reloc_entry->addend);
	}
    }

  return r;
}

static bfd_reloc_status_type
nios2_elf32_ujmp_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **msg ATTRIBUTE_UNUSED)
{
  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_ujmp_relocate (abfd, reloc_entry->howto,
				       input_section,
				       data, reloc_entry->address,
				       (symbol->value
					+ symbol->section->output_section->vma
					+ symbol->section->output_offset),
				       reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_cjmp_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **msg ATTRIBUTE_UNUSED)
{
  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;

  return nios2_elf32_do_cjmp_relocate (abfd, reloc_entry->howto,
				       input_section,
				       data, reloc_entry->address,
				       (symbol->value
					+ symbol->section->output_section->vma
					+ symbol->section->output_offset),
				       reloc_entry->addend);
}

static bfd_reloc_status_type
nios2_elf32_callr_relocate (bfd *abfd, arelent *reloc_entry, asymbol *symbol,
     void *data, asection *input_section, bfd *output_bfd, char **msg ATTRIBUTE_UNUSED)
{
  /* This part is from bfd_elf_generic_reloc.  */
  if (output_bfd != (bfd *) NULL
      && (symbol->flags & BSF_SECTION_SYM) == 0
      && (!reloc_entry->howto->partial_inplace || reloc_entry->addend == 0))
    {
      reloc_entry->address += input_section->output_offset;
      return bfd_reloc_ok;
    }

  if (output_bfd != NULL)
    /* FIXME: See bfd_perform_relocation.  Is this right?  */
    return bfd_reloc_ok;


  return nios2_elf32_do_callr_relocate (abfd, reloc_entry->howto,
					input_section,
					data, reloc_entry->address,
					(symbol->value
					 +
					 symbol->section->output_section->
					 vma +
					 symbol->section->output_offset),
					reloc_entry->addend);
}

/* Do the relocations which require special handling */

static bfd_reloc_status_type
nios2_elf32_do_hi16_relocate (bfd *abfd, reloc_howto_type *howto, 
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_value = (symbol_value >> 16) & 0xffff;
  return _bfd_final_link_relocate (howto, abfd, input_section,
				   data, offset, symbol_value, addend);
}


static bfd_reloc_status_type
nios2_elf32_do_lo16_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_value = symbol_value & 0xffff;
  return _bfd_final_link_relocate (howto, abfd, input_section,
				   data, offset, symbol_value, addend);
}

static bfd_reloc_status_type
nios2_elf32_do_hiadj16_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data, bfd_vma offset,
     bfd_vma symbol_value, bfd_vma addend)
{
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_value =
    ((symbol_value >> 16) & 0xffff) + ((symbol_value >> 15) & 0x01);
  return _bfd_final_link_relocate (howto, abfd, input_section, data, offset,
				   symbol_value, addend);
}

static bfd_reloc_status_type
nios2_elf32_do_pcrel16_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  // NIOS2 pc relative relocations are relative to the next 32-bit instruction so we need
  // to subtract 4 before doing a final_link_relocate
  symbol_value = symbol_value + addend - 4;
  addend = 0;
  return _bfd_final_link_relocate (howto, abfd, input_section,
				   data, offset, symbol_value, addend);
}

static bfd_reloc_status_type
nios2_elf32_do_call26_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  /* check that the relocation is in the same page as the current address */
  if (((symbol_value + addend) & 0xf0000000)
      != ((input_section->output_section->vma + offset) & 0xf0000000))
    return bfd_reloc_overflow;

  return _bfd_final_link_relocate (howto, abfd, input_section,
				   data, offset, symbol_value, addend);
}


static bfd_reloc_status_type
nios2_elf32_do_gprel_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  // because we need the output_bfd, the special handling is done
  // in nios2_elf32_relocate_section or in nios2_elf32_gprel_relocate
  return _bfd_final_link_relocate (howto, abfd, input_section,
				   data, offset, symbol_value, addend);
}

static bfd_reloc_status_type
nios2_elf32_do_ujmp_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  bfd_vma symbol_lo16, symbol_hi16;
  bfd_reloc_status_type r;
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_hi16 = (symbol_value >> 16) & 0xffff;
  symbol_lo16 = symbol_value & 0xffff;

  r = _bfd_final_link_relocate (howto, abfd, input_section,
				data, offset, symbol_hi16, addend);

  if (r == bfd_reloc_ok)
    return _bfd_final_link_relocate (howto, abfd, input_section,
				     data, offset + 4, symbol_lo16, addend);

  return r;
}

static bfd_reloc_status_type
nios2_elf32_do_cjmp_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  bfd_vma symbol_lo16, symbol_hi16;
  bfd_reloc_status_type r;
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_hi16 = (symbol_value >> 16) & 0xffff;
  symbol_lo16 = symbol_value & 0xffff;

  r = _bfd_final_link_relocate (howto, abfd, input_section,
				data, offset, symbol_hi16, addend);

  if (r == bfd_reloc_ok)
    return _bfd_final_link_relocate (howto, abfd, input_section,
				     data, offset + 4, symbol_lo16, addend);

  return r;
}

static bfd_reloc_status_type
nios2_elf32_do_callr_relocate (bfd *abfd, reloc_howto_type *howto,
     asection *input_section ATTRIBUTE_UNUSED, bfd_byte *data,
     bfd_vma offset, bfd_vma symbol_value, bfd_vma addend)
{
  bfd_vma symbol_lo16, symbol_hi16;
  bfd_reloc_status_type r;
  symbol_value = symbol_value + addend;
  addend = 0;
  symbol_hi16 = (symbol_value >> 16) & 0xffff;
  symbol_lo16 = symbol_value & 0xffff;

  r = _bfd_final_link_relocate (howto, abfd, input_section,
				data, offset, symbol_hi16, addend);

  if (r == bfd_reloc_ok)
    return _bfd_final_link_relocate (howto, abfd, input_section,
				     data, offset + 4, symbol_lo16, addend);

  return r;
}

/*
   The function nios2_elf32_relocate_section is used by the linker
   to perform relocations
*/
static bfd_boolean
nios2_elf32_relocate_section (bfd * output_bfd,
			      struct bfd_link_info *info,
			      bfd * input_bfd,
			      asection * input_section,
			      bfd_byte * contents,
			      Elf_Internal_Rela * relocs,
			      Elf_Internal_Sym * local_syms,
			      asection ** local_sections)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes;
  Elf_Internal_Rela *rel;
  Elf_Internal_Rela *relend;

  nios2_link_info = info;       // rdp: A hack to grab the info pointer.
  symtab_hdr = &elf_tdata (input_bfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (input_bfd);
  relend = relocs + input_section->reloc_count;

//  size_t psymalloc = 0;
//  _bfd_generic_link_output_symbols(output_bfd, input_bfd, info, &psymalloc);
  for (rel = relocs; rel < relend; rel++)
    {
      reloc_howto_type *howto;
      unsigned long r_symndx;
      Elf_Internal_Sym *sym;
      asection *sec;
      struct elf_link_hash_entry *h;
      bfd_vma relocation;
      bfd_vma gp;
      bfd_vma reloc_address;
      bfd_reloc_status_type r = bfd_reloc_ok;
      const char *name = NULL;
      int r_type;
      const char *format;
      char msgbuf[256];
      const char* msg = (const char*) NULL;

       

      r_type = ELF32_R_TYPE (rel->r_info);

      r_symndx = ELF32_R_SYM (rel->r_info);

      if (info->relocatable)
	{
	  /* This is a relocatable link.  We don't have to change
	     anything, unless the reloc is against a section symbol,
	     in which case we have to adjust according to where the
	     section symbol winds up in the output section.  */
	  if (r_symndx < symtab_hdr->sh_info)
	    {
	      sym = local_syms + r_symndx;

	      if (ELF_ST_TYPE (sym->st_info) == STT_SECTION)
		{
		  sec = local_sections[r_symndx];
		  rel->r_addend += sec->output_offset + sym->st_value;
		}
	    }
	  continue;
	}

      /* This is a final link.  */
      howto = lookup_howto ((unsigned) ELF32_R_TYPE (rel->r_info));
      h = NULL;
      sym = NULL;
      sec = NULL;

      if (r_symndx < symtab_hdr->sh_info)
	{
	  sym = local_syms + r_symndx;
	  sec = local_sections[r_symndx];

	  relocation = (sec->output_section->vma
			+ sec->output_offset + sym->st_value);

	  // this ensures that relocations against duplicated symbols
	  // in merged sections that have been removed are fixed up against
	  // the remaining symbol and not the one that has been removed
	  if ((sec->flags & SEC_MERGE)
	      && ELF_ST_TYPE (sym->st_info) == STT_SECTION)
	    {
	      rel->r_addend =
		_bfd_elf_rel_local_sym (output_bfd, sym, &sec, rel->r_addend);
	      rel->r_addend -= relocation;
	      rel->r_addend += sec->output_section->vma + sec->output_offset;
	    }

	  name = bfd_elf_string_from_elf_section
	    (input_bfd, symtab_hdr->sh_link, sym->st_name);

	  name = (name == NULL) ? bfd_section_name (input_bfd, sec) : name;
	}
      else
	{
	  h = sym_hashes[r_symndx - symtab_hdr->sh_info];

	  while (h->root.type == bfd_link_hash_indirect
		 || h->root.type == bfd_link_hash_warning)
	    h = (struct elf_link_hash_entry *) h->root.u.i.link;

	  name = h->root.root.string;

	  if (h->root.type == bfd_link_hash_defined
	      || h->root.type == bfd_link_hash_defweak)
	    {
	      sec = h->root.u.def.section;

	      relocation = (h->root.u.def.value
			    + sec->output_section->vma + sec->output_offset);
	    }
	  else if (h->root.type == bfd_link_hash_undefweak)
	    {
	      relocation = 0;
	    }
	  else
	    {
	      if (!((*info->callbacks->undefined_symbol)
		    (info, h->root.root.string, input_bfd,
		     input_section, rel->r_offset, TRUE)))
		return FALSE;
	      relocation = 0;
	    }
	}

	if (sec)
		reloc_address = sec->output_section->vma + sec->output_offset + rel->r_offset;
	else
		reloc_address = 0;
		
      if (howto != NULL)
	{
	  switch (howto->type)
	    {
	    case R_NIOS2_HI16:
	      r =
		nios2_elf32_do_hi16_relocate (input_bfd, howto, input_section,
					      contents, rel->r_offset,
					      relocation, rel->r_addend);
	      break;
	    case R_NIOS2_LO16:
	      r =
		nios2_elf32_do_lo16_relocate (input_bfd, howto, input_section,
					      contents, rel->r_offset,
					      relocation, rel->r_addend);
	      break;
	    case R_NIOS2_HIADJ16:
	      r =
		nios2_elf32_do_hiadj16_relocate (input_bfd, howto,
						 input_section, contents,
						 rel->r_offset, relocation,
						 rel->r_addend);
	      break;
	    case R_NIOS2_PCREL16:
	      r =
		nios2_elf32_do_pcrel16_relocate (input_bfd, howto,
						 input_section, contents,
						 rel->r_offset, relocation,
						 rel->r_addend);
	      break;
	    case R_NIOS2_GPREL:
	      // turns an absolute address into a gp-relative address
	      if (!nios2_elf_assign_gp (output_bfd, &gp, info))
		{
		  format = _("global pointer relative relocation at address 0x%08x when _gp not defined\n");
		  sprintf(msgbuf, format, reloc_address);
		  msg = msgbuf;
		  r = bfd_reloc_dangerous;
		}
	      else
		{
		  bfd_vma symbol_address = rel->r_addend + relocation;
		  relocation = relocation + rel->r_addend - gp;
		  rel->r_addend = 0;
		  if ((signed) relocation < -32768
		      || (signed) relocation > 32767)
		    {
		      format = _("Unable to reach %s (at 0x%08x) from the global pointer (at 0x%08x) "
				 "because the offset (%d) is out of the allowed range, -32678 to 32767.\n" );
		      sprintf(msgbuf, format, name, symbol_address, gp, (signed)relocation);
		      msg = msgbuf;
		      r = bfd_reloc_outofrange;
		    }
		  else
		    {
		      r =
			_bfd_final_link_relocate (howto, input_bfd,
						  input_section, contents,
						  rel->r_offset, relocation,
						  rel->r_addend);
		    }
		}

	      break;
	    case R_NIOS2_UJMP:
	      r =
		nios2_elf32_do_ujmp_relocate (input_bfd, howto, input_section,
					      contents, rel->r_offset,
					      relocation, rel->r_addend);
	      break;
	    case R_NIOS2_CJMP:
	      r =
		nios2_elf32_do_cjmp_relocate (input_bfd, howto, input_section,
					      contents, rel->r_offset,
					      relocation, rel->r_addend);
	      break;
	    case R_NIOS2_CALLR:
	      r =
		nios2_elf32_do_callr_relocate (input_bfd, howto,
					       input_section, contents,
					       rel->r_offset, relocation,
					       rel->r_addend);
	      break;
	    case R_NIOS2_CALL26:
	      r =
		nios2_elf32_do_call26_relocate (input_bfd, howto,
						input_section, contents,
						rel->r_offset, relocation,
						rel->r_addend);
	      break;
	    case R_NIOS2_ALIGN:
	      r = bfd_reloc_ok;
	      /* comment - for symmetry this would be
	      		r = nios2_elf32_do_ignore_reloc (input_bfd, howto, input_section,
										    contents, rel->r_offset,
					    					relocation, rel->r_addend);
		  	 but do_ignore_reloc would do no more than return bfd_reloc_ok */
	      break;
	    default:
	      r = _bfd_final_link_relocate (howto, input_bfd, input_section,
					    contents, rel->r_offset,
					    relocation, rel->r_addend);
	      break;
	    }
	}
      else
	{
	  r = bfd_reloc_notsupported;
	}

      if (r != bfd_reloc_ok)
	{
	  if (h != NULL)
	    name = h->root.root.string;
	  else
	    {
	      name = (bfd_elf_string_from_elf_section
		      (input_bfd, symtab_hdr->sh_link, sym->st_name));
	      if (name == NULL || *name == '\0')
		name = bfd_section_name (input_bfd, sec);
	    }

	  switch (r)
	    {
	    case bfd_reloc_overflow:
	      r = info->callbacks->reloc_overflow
		(info, (h ? &h->root : NULL), name, howto->name, (bfd_vma) 0,
		 input_bfd, input_section, rel->r_offset);
	      break;

	    case bfd_reloc_undefined:
	      r = info->callbacks->undefined_symbol
		(info, name, input_bfd, input_section, rel->r_offset, TRUE);
	      break;

	    case bfd_reloc_outofrange:
	      if (msg == NULL)
		msg = _("relocation out of range");
	      break;

	    case bfd_reloc_notsupported:
	      if (msg == NULL)
		msg = _("unsupported relocation");
	      break;

	    case bfd_reloc_dangerous:
	      if (msg == NULL)
		msg = _("dangerous relocation");
	      break;

	    default:
	      if (msg == NULL)
		msg = _("unknown error");
	      break;
	    }

	  if (msg)
	    {
	      r = info->callbacks->warning
		(info, msg, name, input_bfd, input_section, rel->r_offset);
	      return FALSE;
	    }
	}
    }
  return TRUE;
}



/* Handle an NIOS2 specific section when reading an object file.  This
   is called when elfcode.h finds a section with an unknown type.
   FIXME: We need to handle the SHF_NIOS2_GPREL flag */

static bfd_boolean
nios2_elf32_section_from_shdr (bfd *abfd,
     Elf_Internal_Shdr *hdr, const char *name, int shindex)
{
  asection *newsect;

  /* NG - I'm keeping this code commented out at the moment
     in case we add a .mdebug section */

  /*
     switch (hdr->sh_type)
     {
     case SHT_NIOS2_DEBUG:
     if (strcmp (name, ".mdebug") != 0)
     return FALSE;
     break;
     default:
     return FALSE;
     }
   */

  if (!_bfd_elf_make_section_from_shdr (abfd, hdr, name, shindex))
    return FALSE;

  newsect = hdr->bfd_section;

  /* ditto */
  /*
     if (hdr->sh_type == SHT_NIOS2_DEBUG)
     {
     if (! bfd_set_section_flags (abfd, newsect,
     (bfd_get_section_flags (abfd, newsect)
     | SEC_DEBUGGING)))
     return FALSE;
     }
   */
  return TRUE;
}

/* Convert NIOS2 specific section flags to bfd internal section flags.  */

static bfd_boolean
nios2_elf32_section_flags (flagword *flags, const Elf_Internal_Shdr *hdr)
{
  if (hdr->sh_flags & SHF_NIOS2_GPREL)
    *flags |= SEC_SMALL_DATA;

  return TRUE;
}

/* Set the correct type for an NIOS2 ELF section.  We do this by the
   section name, which is a hack, but ought to work.  */

static bfd_boolean
nios2_elf32_fake_sections (bfd *abfd ATTRIBUTE_UNUSED,
     Elf_Internal_Shdr *hdr, asection *sec)
{
  register const char *name;

  name = bfd_get_section_name (abfd, sec);

  if (strcmp (name, ".mdebug") == 0)
    {
      /* we don't yet have an .mdebug section, but I'm leaving this here
         in case we ever do
         hdr->sh_type = SHT_NIOS2_DEBUG;

         if ((abfd->flags & DYNAMIC) != 0 )
         hdr->sh_entsize = 0;
         else
         hdr->sh_entsize = 1;
       */
    }
  else if ((sec->flags & SEC_SMALL_DATA)
	   || strcmp (name, ".sdata") == 0
	   || strcmp (name, ".sbss") == 0
	   || strcmp (name, ".lit4") == 0 || strcmp (name, ".lit8") == 0)
    hdr->sh_flags |= SHF_NIOS2_GPREL;

  return TRUE;
}

/* Look through the relocs for a section during the first phase.
   Since we don't do .gots or .plts, we just need to consider the
   virtual table relocs for gc.  */

static bfd_boolean
nios2_elf32_check_relocs (bfd *abfd, struct bfd_link_info *info,
     asection *sec, const Elf_Internal_Rela *relocs)
{
  Elf_Internal_Shdr *symtab_hdr;
  struct elf_link_hash_entry **sym_hashes, **sym_hashes_end;
  const Elf_Internal_Rela *rel;
  const Elf_Internal_Rela *rel_end;

  nios2_link_info = info;       // rdp: A hack to grab the info pointer.
  if (info->relocatable)
    return TRUE;

  symtab_hdr = &elf_tdata (abfd)->symtab_hdr;
  sym_hashes = elf_sym_hashes (abfd);
  sym_hashes_end =
    sym_hashes + symtab_hdr->sh_size / sizeof (Elf32_External_Sym);
  if (!elf_bad_symtab (abfd))
    sym_hashes_end -= symtab_hdr->sh_info;

  rel_end = relocs + sec->reloc_count;
  for (rel = relocs; rel < rel_end; rel++)
    {
      struct elf_link_hash_entry *h;
      unsigned long r_symndx;

      r_symndx = ELF32_R_SYM (rel->r_info);
      if (r_symndx < symtab_hdr->sh_info)
	h = NULL;
      else
	h = sym_hashes[r_symndx - symtab_hdr->sh_info];

      switch (ELF32_R_TYPE (rel->r_info))
	{
	  /* This relocation describes the C++ object vtable hierarchy.
	     Reconstruct it for later use during GC.  */
	case R_NIOS2_GNU_VTINHERIT:
	  if (!bfd_elf_gc_record_vtinherit (abfd, sec, h, rel->r_offset))
	    return FALSE;
	  break;

	  /* This relocation describes which C++ vtable entries are actually
	     used.  Record for later use during GC.  */
	case R_NIOS2_GNU_VTENTRY:
	  if (!bfd_elf_gc_record_vtentry (abfd, sec, h, rel->r_addend))
	    return FALSE;
	  break;
	}
    }

  return TRUE;
}


/* Return the section that should be marked against GC for a given
   relocation.  */

asection *
nios2_elf32_gc_mark_hook (asection *sec,
     struct bfd_link_info *info ATTRIBUTE_UNUSED,
     Elf_Internal_Rela *rel, struct elf_link_hash_entry *h,
     Elf_Internal_Sym *sym)
{
  if (h != NULL)
    {
      switch (ELF32_R_TYPE (rel->r_info))
	{
	case R_NIOS2_GNU_VTINHERIT:
	case R_NIOS2_GNU_VTENTRY:
	  break;

	default:
	  switch (h->root.type)
	    {
	    case bfd_link_hash_defined:
	    case bfd_link_hash_defweak:
	      return h->root.u.def.section;

	    case bfd_link_hash_common:
	      return h->root.u.c.p->section;

	    default:
	      break;
	    }
	}
    }
  else
    return bfd_section_from_elf_index (sec->owner, sym->st_shndx);

  return NULL;
}

/*
	NG ??? I'm marking the sections as standalone ie. I'm linking for
	standalone embedded applications, not for UNIX System V or any other
	OS/ABI - this may need to change when we deal with embedded PIC or
	dynamic linking
*/

static void
nios2_elf32_post_process_headers (bfd *abfd,
     struct bfd_link_info *link_info ATTRIBUTE_UNUSED)
{
  Elf_Internal_Ehdr *i_ehdrp;	/* Elf file header, internal form */

  i_ehdrp = elf_elfheader (abfd);
  i_ehdrp->e_ident[EI_OSABI] = ELFOSABI_STANDALONE;
}

#define ELF_ARCH                            bfd_arch_nios2
#define ELF_MACHINE_CODE                EM_ALTERA_NIOS2

/* for now we just make this 1, as we have no MMU in New Jersey */

#define ELF_MAXPAGESIZE                 1

/* relocation table lookup macros */

#define bfd_elf32_bfd_reloc_type_lookup   nios2_elf32_bfd_reloc_type_lookup
#define bfd_elf32_bfd_reloc_name_lookup   nios2_elf32_bfd_reloc_name_lookup

/* JUMP_TABLE_LINK macros */

#define bfd_elf32_bfd_relax_section		  nios2_elf32_relax_section

/* elf_info_to_howto (using RELA relocations) */

#define elf_info_to_howto                 nios2_elf32_info_to_howto

/* elf backend functions */

#define elf_backend_can_gc_sections	1

#define elf_backend_relocate_section      nios2_elf32_relocate_section
#define elf_backend_section_from_shdr     nios2_elf32_section_from_shdr
#define elf_backend_section_flags         nios2_elf32_section_flags
#define elf_backend_fake_sections         nios2_elf32_fake_sections
#define elf_backend_post_process_headers  nios2_elf32_post_process_headers
#define elf_backend_check_relocs		  nios2_elf32_check_relocs

#define elf_backend_gc_mark_hook		  nios2_elf32_gc_mark_hook



/* Support for SGI-ish mips targets. */
#define TARGET_LITTLE_SYM               bfd_elf32_littlenios2_vec
#define TARGET_LITTLE_NAME              "elf32-littlenios2"
#define TARGET_BIG_SYM                  bfd_elf32_bignios2_vec
#define TARGET_BIG_NAME                 "elf32-bignios2"

#include "elf32-target.h"
