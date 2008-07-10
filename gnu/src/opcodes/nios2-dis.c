/* nios2-dis.c -- Altera New Jersey disassemble routines.

   Copyright (C) 2003
   by Nigel Gray (ngray@altera.com).

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "dis-asm.h"
#include "opcode/nios2.h"

/* No symbol table is available when this code runs out in an embedded
   system as when it is used for disassembler support in a monitor. */

#if !defined(EMBEDDED_ENV)
#define SYMTAB_AVAILABLE 1
#include "elf-bfd.h"
#include "elf/nios2.h"
#endif

/* length of New Jersey instruction in bytes */
#define INSNLEN 4

/* helper function prototypes */
static int nios2_disassemble (bfd_vma, unsigned long, disassemble_info *);
static void nios2_init_opcode_hash (void);


static int nios2_print_insn_arg (const char *argptr, unsigned long opcode,
				 bfd_vma address, disassemble_info * info);


/* print_insn_nios2 is the main disassemble function for New Jersey.
   The function diassembler(abfd) (source in disassemble.c) returns a
   pointer to this either print_insn_big_nios2 or
   print_insn_little_nios2, which in turn call this function, when the
   bfd machine type is New Jersey. print_insn_nios2 reads the
   instruction word at the address given, and prints the disassembled
   instruction on the stream info->stream using info->fprintf_func. */

static int
print_insn_nios2 (bfd_vma address, disassemble_info * info,
                  enum bfd_endian endianness)
{
  /* buffer into which the instruction bytes are written */
  bfd_byte buffer[INSNLEN];
  /* used to indicate return status from function calls */
  int status;

  assert (info != NULL);

  status = (*info->read_memory_func) (address, buffer, INSNLEN, info);
  if (status == 0)
    {
      unsigned long insn;
      if (endianness == BFD_ENDIAN_BIG)
        insn = (unsigned long) bfd_getb32 (buffer);
      else
        insn = (unsigned long) bfd_getl32 (buffer);
      status = nios2_disassemble (address, insn, info);
    }
  else
    {
      (*info->memory_error_func) (status, address, info);
      status = -1;
    }
  return status;
}

int
print_insn_big_nios2 (bfd_vma address, disassemble_info * info)
{
  return print_insn_nios2 (address, info, BFD_ENDIAN_BIG);
}

int
print_insn_little_nios2 (bfd_vma address, disassemble_info * info)
{
  return print_insn_nios2 (address, info, BFD_ENDIAN_LITTLE);
}

/* Data structures used by the opcode hash table */

typedef struct _nios2_opcode_hash
{
  const struct nios2_opcode *opcode;
  struct _nios2_opcode_hash *next;
} nios2_opcode_hash;

static bfd_boolean nios2_hash_init = 0;
static nios2_opcode_hash *nios2_hash[(OP_MASK_OP) + 1];

/* separate hash table for pseudo-ops */
static nios2_opcode_hash *nios2_ps_hash[(OP_MASK_OP) + 1];

/* Function to initialize the opcode hash table */

void
nios2_init_opcode_hash ()
{
  unsigned int i;
  register const struct nios2_opcode *op;
  nios2_opcode_hash *tmp_hash;

  for (i = 0; i <= OP_MASK_OP; ++i)
    {
      nios2_hash[0] = NULL;
    }
  for (i = 0; i <= OP_MASK_OP; i++)
    {
      for (op = nios2_opcodes; op < &nios2_opcodes[NUMOPCODES]; op++)
	{
	  if ((op->pinfo & NIOS2_INSN_MACRO) == NIOS2_INSN_MACRO)
	    {
	      if (i == ((op->match >> OP_SH_OP) & OP_MASK_OP) &&
		  (op->
		   pinfo & (NIOS2_INSN_MACRO_MOV | NIOS2_INSN_MACRO_MOVI) &
		   0x7fffffff) != 0)
		{
		  tmp_hash = nios2_ps_hash[i];
		  if (tmp_hash == NULL)
		    {
		      tmp_hash =
			(nios2_opcode_hash *)
			malloc (sizeof (nios2_opcode_hash));
		      nios2_ps_hash[i] = tmp_hash;
		    }
		  else
		    {
		      while (tmp_hash->next != NULL)
			tmp_hash = tmp_hash->next;
		      tmp_hash->next =
			(nios2_opcode_hash *)
			malloc (sizeof (nios2_opcode_hash));
		      tmp_hash = tmp_hash->next;
		    }
		  if (tmp_hash == NULL)
		    {
		      fprintf (stderr,
			       "error allocating memory...broken disassembler\n");
		      abort ();
		    }
		  tmp_hash->opcode = op;
		  tmp_hash->next = NULL;
		}
	    }
	  else if (i == ((op->match >> OP_SH_OP) & OP_MASK_OP))
	    {
	      tmp_hash = nios2_hash[i];
	      if (tmp_hash == NULL)
		{
		  tmp_hash =
		    (nios2_opcode_hash *) malloc (sizeof (nios2_opcode_hash));
		  nios2_hash[i] = tmp_hash;
		}
	      else
		{
		  while (tmp_hash->next != NULL)
		    tmp_hash = tmp_hash->next;
		  tmp_hash->next =
		    (nios2_opcode_hash *) malloc (sizeof (nios2_opcode_hash));
		  tmp_hash = tmp_hash->next;
		}
	      if (tmp_hash == NULL)
		{
		  fprintf (stderr,
			   "error allocating memory...broken disassembler\n");
		  abort ();
		}
	      tmp_hash->opcode = op;
	      tmp_hash->next = NULL;
	    }
	}
    }
  nios2_hash_init = 1;
#ifdef DEBUG_HASHTABLE
  for (i = 0; i <= OP_MASK_OP; ++i)
    {
      printf ("index: 0x%02X    ops: ", i);
      tmp_hash = nios2_hash[i];
      if (tmp_hash != NULL)
	{
	  while (tmp_hash != NULL)
	    {
	      printf ("%s ", tmp_hash->opcode->name);
	      tmp_hash = tmp_hash->next;
	    }
	}
      printf ("\n");
    }

  for (i = 0; i <= OP_MASK_OP; ++i)
    {
      printf ("index: 0x%02X    ops: ", i);
      tmp_hash = nios2_ps_hash[i];
      if (tmp_hash != NULL)
	{
	  while (tmp_hash != NULL)
	    {
	      printf ("%s ", tmp_hash->opcode->name);
	      tmp_hash = tmp_hash->next;
	    }
	}
      printf ("\n");
    }
#endif
}

/* Function which returns a pointer to an nios2_opcode struct for
   a given instruction opcode, or NULL if there is an error */

const struct nios2_opcode *
nios2_find_opcode_hash (unsigned long opcode)
{
  nios2_opcode_hash *entry;

  /* Build a hash table to shorten the search time. */
  if (!nios2_hash_init)
    {
      nios2_init_opcode_hash ();
    }

  /* first look in the pseudo-op hashtable */
  entry = nios2_ps_hash[(opcode >> OP_SH_OP) & OP_MASK_OP];

  /* look for a match and if we get one, this is the instruction we decode */
  while (entry != NULL)
    {
      if ((entry->opcode->match) == (opcode & entry->opcode->mask))
	return entry->opcode;
      else
	entry = entry->next;
    }

  /* if we haven't yet returned, then we need to look in the main
     hashtable */
  entry = nios2_hash[(opcode >> OP_SH_OP) & OP_MASK_OP];

  if (entry == NULL)
    return NULL;


  while (entry != NULL)
    {
      if ((entry->opcode->match) == (opcode & entry->opcode->mask))
	return entry->opcode;
      else
	entry = entry->next;
    }

  return NULL;
}

/* nios2_disassemble does all the work of disassembling a New Jersey
       instruction opcode */

int
nios2_disassemble (bfd_vma address, unsigned long opcode,
		   disassemble_info * info)
{
  const struct nios2_opcode *op;
  const char *argstr;

  info->bytes_per_line = INSNLEN;
  info->bytes_per_chunk = INSNLEN;
  info->display_endian = info->endian;
  info->insn_info_valid = 1;
  info->branch_delay_insns = 0;
  info->data_size = 0;
  info->insn_type = dis_nonbranch;
  info->target = 0;
  info->target2 = 0;

  /* Find the major opcode and use this to disassemble
     the instruction and its arguments */
  op = nios2_find_opcode_hash (opcode);

  if (op != NULL)
    {
      bfd_boolean is_nop = FALSE;
      if (op->pinfo == NIOS2_INSN_MACRO_MOV)
	{
	  /* check for mov r0, r0 and if it is
	     change to nop */
	  int dst, src;
	  dst = GET_INSN_FIELD (RRD, opcode);
	  src = GET_INSN_FIELD (RRS, opcode);
	  if (dst == 0 && src == 0)
	    {
	      (*info->fprintf_func) (info->stream, "nop");
	      is_nop = TRUE;
	    }
	  else
	    {
	      (*info->fprintf_func) (info->stream, "%s", op->name);
	    }
	}
      else
	{
	  (*info->fprintf_func) (info->stream, "%s", op->name);
	}

      if (!is_nop)
	{
	  argstr = op->args;
	  if (argstr != NULL && *argstr != '\0')
	    {
	      (*info->fprintf_func) (info->stream, "\t");
	      while (*argstr != '\0')
		{
		  nios2_print_insn_arg (argstr, opcode, address, info);
		  ++argstr;
		}
	    }
	}
    }
  else
    {
      /* Handle undefined instructions. */
      info->insn_type = dis_noninsn;
      (*info->fprintf_func) (info->stream, "0x%x", opcode);
    }
  // this tells the caller how far to advance the program counter
  return INSNLEN;
}

/* The function nios2_print_insn_arg uses the character pointed
   to by argptr to determine how it print the next token or separator
   character in the arguments to an instruction */
int
nios2_print_insn_arg (const char *argptr,
		      unsigned long opcode, bfd_vma address,
		      disassemble_info * info)
{
  unsigned long i = 0;
  unsigned long reg_base;

  assert (argptr != NULL);
  assert (info != NULL);

  switch (*argptr)
    {
    case ',':
    case '(':
    case ')':
      (*info->fprintf_func) (info->stream, "%c", *argptr);
      break;
    case 'd':
      i = GET_INSN_FIELD (RRD, opcode);

      if (GET_INSN_FIELD (OP, opcode) == OP_MATCH_CUSTOM
	  && GET_INSN_FIELD (CUSTOM_C, opcode) == 0)
	reg_base = COPROCREGBASE;
      else
	reg_base = 0;

      if (i < NUMREGNAMES)
	(*info->fprintf_func) (info->stream, "%s",
			       nios2_regs[i + reg_base].name);
      else
	(*info->fprintf_func) (info->stream, "unknown");
      break;
    case 's':
      i = GET_INSN_FIELD (RRS, opcode);

      if (GET_INSN_FIELD (OP, opcode) == OP_MATCH_CUSTOM
	  && GET_INSN_FIELD (CUSTOM_A, opcode) == 0)
	reg_base = COPROCREGBASE;
      else
	reg_base = 0;

      if (i < NUMREGNAMES)
	(*info->fprintf_func) (info->stream, "%s",
			       nios2_regs[i + reg_base].name);
      else
	(*info->fprintf_func) (info->stream, "unknown");
      break;
    case 't':
      i = GET_INSN_FIELD (RRT, opcode);

      if (GET_INSN_FIELD (OP, opcode) == OP_MATCH_CUSTOM
	  && GET_INSN_FIELD (CUSTOM_B, opcode) == 0)
	reg_base = COPROCREGBASE;
      else
	reg_base = 0;

      if (i < NUMREGNAMES)
	(*info->fprintf_func) (info->stream, "%s",
			       nios2_regs[i + reg_base].name);
      else
	(*info->fprintf_func) (info->stream, "unknown");
      break;
    case 'i':
      /* 16-bit signed immediate */
      i = (signed) (GET_INSN_FIELD (IMM16, opcode) << 16) >> 16;
      (*info->fprintf_func) (info->stream, "%d", i);
      break;
    case 'u':
      /* 16-bit unsigned immediate */
      i = GET_INSN_FIELD (IMM16, opcode);
      (*info->fprintf_func) (info->stream, "%d", i);
      break;
    case 'o':
      /* 16-bit signed immediate address offset */
      i = (signed) (GET_INSN_FIELD (IMM16, opcode) << 16) >> 16;
      address = address + 4 + i;	/* NG changed to byte offset 1/9/03 */
      (*info->print_address_func) (address, info);
      break;
    case 'p':
      /* 5-bit unsigned immediate */
      i = GET_INSN_FIELD (CACHE_OPX, opcode);
      (*info->fprintf_func) (info->stream, "%d", i);
      break;
    case 'j':
      /* 5-bit unsigned immediate */
      i = GET_INSN_FIELD (IMM5, opcode);
      (*info->fprintf_func) (info->stream, "%d", i);
      break;
    case 'l':
      /* 8-bit unsigned immediate */
      /* FIXME - not yet implemented */
      i = GET_INSN_FIELD (CUSTOM_N, opcode);
      (*info->fprintf_func) (info->stream, "%u", i);
      break;
    case 'm':
      /* 26-bit unsigned immediate */
      i = GET_INSN_FIELD (IMM26, opcode);
      /* this translates to an address because its only used in call instructions */
      address = (address & 0xf0000000) | (i << 2);
      (*info->print_address_func) (address, info);
      break;
    case 'c':
      i = GET_INSN_FIELD (IMM5, opcode);	/* ctrl register index */
      (*info->fprintf_func) (info->stream, "%s",
			     nios2_regs[CTLREGBASE + i].name);
      break;
    case 'b':
      i = GET_INSN_FIELD (IMM5, opcode);
      (*info->fprintf_func) (info->stream, "%d", i);
      break;
    default:
      (*info->fprintf_func) (info->stream, "unknown");
      break;
    }
  return 0;
}
