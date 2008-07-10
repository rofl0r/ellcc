/* nios2.h.  Altera New Jersey opcode list for GAS, the GNU assembler.

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

#ifndef _NIOS2_H_
#define _NIOS2_H_


/****************************************************************************
 * This file contains structures, bit masks and shift counts used
 * by the GNU toolchain to define the New Jersey instruction set and
 * access various opcode fields.
 ****************************************************************************/

enum overflow_type
{
  call_target_overflow = 0,
  branch_target_overflow,
  address_offset_overflow,
  signed_immed16_overflow,
  unsigned_immed16_overflow,
  unsigned_immed5_overflow,
  custom_opcode_overflow,
  no_overflow
};

/*---------------------------------------------------------------------------
   This structure holds information for a particular instruction
  ---------------------------------------------------------------------------*/

/* match When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is 0.  */

/* mask If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */

/* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */

struct nios2_opcode
{
  const char *name;		/* The name of the instruction.  */
  const char *args;		/* A string describing the arguments for this instruction.  */
  const char *args_test;	/* Like args, but with an extra argument for the expected opcode */
  unsigned long num_args;	/* the number of arguments the instruction takes */
  unsigned long match;		/* The basic opcode for the instruction. */
  unsigned long mask;		/* mask for the opcode field of the instruction */
  unsigned long pinfo;		/* is this a real instruction or instruction macro */
  enum overflow_type overflow_msg;	/* msg template used to generate informative message when fixup overflows */
};

/* This value is used in the nios2_opcode.pinfo field to indicate that the instruction
   is a macro or pseudo-op. This requires special treatment by the assembler, and is
   used by the disassembler to determine whether to check for a nop */
#define NIOS2_INSN_MACRO 		0x80000000
#define NIOS2_INSN_MACRO_MOV	0x80000001
#define NIOS2_INSN_MACRO_MOVI	0x80000002
#define NIOS2_INSN_MACRO_MOVIA  0x80000004

#define NIOS2_INSN_RELAXABLE	0x40000000
#define NIOS2_INSN_UBRANCH		0x00000010
#define NIOS2_INSN_CBRANCH		0x00000020
#define NIOS2_INSN_CALL			0x00000040

#define NIOS2_INSN_ADDI			0x00000080
#define NIOS2_INSN_ANDI			0x00000100
#define NIOS2_INSN_ORI			0x00000200
#define NIOS2_INSN_XORI			0x00000400



/* Associates a register name ($6) with a 5-bit index (eg 6) */
struct nios2_reg
{
  const char *name;
  const int index;
};


/* -------------------------------------------------------------------------
    Bitfield masks for New Jersey instructions
   -------------------------------------------------------------------------*/

/* These are bit masks and shift counts to use to access the various
   fields of an instruction. */

/* Macros for getting and setting an instruction field */
#define GET_INSN_FIELD(X, i)     ((i) & OP_MASK_##X) >> OP_SH_##X
#define SET_INSN_FIELD(X, i, j)  (i) = ((i) &~ (OP_MASK_##X)) | ((j) << OP_SH_##X)


/*
   We include the auto-generated file nios2-isa.h and define the mask
   and shifts below in terms of those in nios2-isa.h. This ensures
   that the binutils and hardware are always in sync
*/

#include "nios2-isa.h"

#define OP_MASK_OP              (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_OP                IW_OP_LSB


/* Masks and shifts for I-type instructions */

#define OP_MASK_IOP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_IOP               IW_OP_LSB

#define OP_MASK_IMM16   		(IW_IMM16_MASK << IW_IMM16_LSB)
#define OP_SH_IMM16             IW_IMM16_LSB

#define OP_MASK_IRD             (IW_B_MASK << IW_B_LSB)	// the same as T for I-type
#define OP_SH_IRD               IW_B_LSB

#define OP_MASK_IRT             (IW_B_MASK << IW_B_LSB)
#define OP_SH_IRT               IW_B_LSB

#define OP_MASK_IRS             (IW_A_MASK << IW_A_LSB)
#define OP_SH_IRS               IW_A_LSB

/* Masks and shifts for R-type instructions */

#define OP_MASK_ROP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_ROP               IW_OP_LSB

#define OP_MASK_ROPX    		(IW_OPX_MASK << IW_OPX_LSB)
#define OP_SH_ROPX              IW_OPX_LSB

#define OP_MASK_RRD             (IW_C_MASK << IW_C_LSB)
#define OP_SH_RRD               IW_C_LSB

#define OP_MASK_RRT             (IW_B_MASK << IW_B_LSB)
#define OP_SH_RRT               IW_B_LSB

#define OP_MASK_RRS             (IW_A_MASK << IW_A_LSB)
#define OP_SH_RRS               IW_A_LSB

/* Masks and shifts for J-type instructions */

#define OP_MASK_JOP             (IW_OP_MASK << IW_OP_LSB)
#define OP_SH_JOP               IW_OP_LSB

#define OP_MASK_IMM26   		(IW_IMM26_MASK << IW_IMM26_LSB)
#define OP_SH_IMM26             IW_IMM26_LSB

/* Masks and shifts for CTL instructions */

#define OP_MASK_RCTL    0x000007c0
#define OP_SH_RCTL              6

/* break instruction imm5 field */
#define OP_MASK_TRAP_IMM5 0x000007c0
#define OP_SH_TRAP_IMM5   6

/* instruction imm5 field */
#define OP_MASK_IMM5    		(IW_SHIFT_IMM5_MASK << IW_SHIFT_IMM5_LSB)
#define OP_SH_IMM5              IW_SHIFT_IMM5_LSB

/* cache operation fields (type j,i(s)) */
#define OP_MASK_CACHE_OPX       (IW_B_MASK << IW_B_LSB)
#define OP_SH_CACHE_OPX         IW_B_LSB
#define OP_MASK_CACHE_RRS       (IW_A_MASK << IW_A_LSB)
#define OP_SH_CACHE_RRS         IW_A_LSB

/* custom instruction masks */
#define OP_MASK_CUSTOM_A		0x00010000
#define OP_SH_CUSTOM_A				16

#define OP_MASK_CUSTOM_B		0x00008000
#define OP_SH_CUSTOM_B				15

#define OP_MASK_CUSTOM_C		0x00004000
#define OP_SH_CUSTOM_C				14

#define OP_MASK_CUSTOM_N		0x00003fc0
#define OP_SH_CUSTOM_N				6
#define OP_MAX_CUSTOM_N				255

/*
       The following macros define the opcode matches for each
       instruction
       code & OP_MASK_INST == OP_MATCH_INST
 */

/* OP instruction matches */
#define OP_MATCH_ADDI           OP_ADDI
#define OP_MATCH_ANDHI          OP_ANDHI
#define OP_MATCH_ANDI           OP_ANDI
#define OP_MATCH_BEQ            OP_BEQ
#define OP_MATCH_BGE            OP_BGE
#define OP_MATCH_BGEU           OP_BGEU
#define OP_MATCH_BLT            OP_BLT
#define OP_MATCH_BLTU           OP_BLTU
#define OP_MATCH_BNE            OP_BNE
#define OP_MATCH_BR             OP_BR
#define OP_MATCH_FLUSHD         OP_FLUSHD
#define OP_MATCH_FLUSHDA        OP_FLUSHDA
#define OP_MATCH_INITD          OP_INITD
#define OP_MATCH_CALL           OP_CALL
#define OP_MATCH_CMPEQI         OP_CMPEQI
#define OP_MATCH_CMPGEI         OP_CMPGEI
#define OP_MATCH_CMPGEUI        OP_CMPGEUI
#define OP_MATCH_CMPLTI         OP_CMPLTI
#define OP_MATCH_CMPLTUI        OP_CMPLTUI
#define OP_MATCH_CMPNEI         OP_CMPNEI
#define OP_MATCH_JMPI           OP_JMPI
#define OP_MATCH_LDB            OP_LDB
#define OP_MATCH_LDBIO          OP_LDBIO
#define OP_MATCH_LDBU           OP_LDBU
#define OP_MATCH_LDBUIO         OP_LDBUIO
#define OP_MATCH_LDH            OP_LDH
#define OP_MATCH_LDHIO          OP_LDHIO
#define OP_MATCH_LDHU           OP_LDHU
#define OP_MATCH_LDHUIO         OP_LDHUIO
#define OP_MATCH_LDW            OP_LDW
#define OP_MATCH_LDWIO          OP_LDWIO
#define OP_MATCH_MULI           OP_MULI
#define OP_MATCH_OPX            OP_OPX
#define OP_MATCH_ORHI           OP_ORHI
#define OP_MATCH_ORI            OP_ORI
#define OP_MATCH_STB            OP_STB
#define OP_MATCH_STBIO          OP_STBIO
#define OP_MATCH_STH            OP_STH
#define OP_MATCH_STHIO          OP_STHIO
#define OP_MATCH_STW            OP_STW
#define OP_MATCH_STWIO          OP_STWIO
#define OP_MATCH_CUSTOM         OP_CUSTOM
#define OP_MATCH_XORHI          OP_XORHI
#define OP_MATCH_XORI           OP_XORI
#define OP_MATCH_OPX            OP_OPX



/* OPX instruction values */
#define OP_MATCH_ADD            ((OPX_ADD << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_AND            ((OPX_AND << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_BREAK          ((0x1e << 17) | (OPX_BREAK << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_BRET           ((0xf0000000) | (OPX_BRET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CALLR          ((0x1f << 17) | (OPX_CALLR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPEQ          ((OPX_CMPEQ << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPGE          ((OPX_CMPGE << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPGEU         ((OPX_CMPGEU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPLT          ((OPX_CMPLT << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPLTU         ((OPX_CMPLTU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_CMPNE          ((OPX_CMPNE << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_DIV            ((OPX_DIV << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_DIVU           ((OPX_DIVU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_JMP            ((OPX_JMP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MUL            ((OPX_MUL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXSS         ((OPX_MULXSS << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXSU         ((OPX_MULXSU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_MULXUU         ((OPX_MULXUU << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_NEXTPC         ((OPX_NEXTPC << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_NOR            ((OPX_NOR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_OR             ((OPX_OR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_RDCTL          ((OPX_RDCTL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_RET            ((0xf8000000) | (OPX_RET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROL            ((OPX_ROL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROLI           ((OPX_ROLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ROR            ((OPX_ROR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SLL            ((OPX_SLL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SLLI           ((OPX_SLLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRA            ((OPX_SRA << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRAI           ((OPX_SRAI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRL            ((OPX_SRL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SRLI           ((OPX_SRLI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SUB            ((OPX_SUB << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_SYNC           ((OPX_SYNC << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_TRAP           ((0x1d << 17) | (OPX_TRAP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_ERET           ((0xe8000000) | (OPX_ERET << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_WRCTL          ((OPX_WRCTL << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_XOR            ((OPX_XOR << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_FLUSHI			((OPX_FLUSHI << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_FLUSHP			((OPX_FLUSHP << IW_OPX_LSB) | (OP_OPX))
#define OP_MATCH_INITI			((OPX_INITI << IW_OPX_LSB) | (OP_OPX))

/*
       Some unusual op masks
*/
#define OP_MASK_BREAK           ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP) & 0xfffff03f)
#define OP_MASK_CALLR           ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_JMP             ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SYNC            ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_TRAP            ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP) & 0xfffff83f)
#define OP_MASK_WRCTL           ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))	/*& 0xfffff83f */
#define OP_MASK_NEXTPC          ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_FLUSHI          ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_INITI           ((OP_MASK_RRT | OP_MASK_RRD | OP_MASK_ROPX | OP_MASK_OP))

#define OP_MASK_ROLI            ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SLLI            ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SRAI            ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_SRLI            ((OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))
#define OP_MASK_RDCTL           ((OP_MASK_RRS | OP_MASK_RRT | OP_MASK_ROPX | OP_MASK_OP))	/*& 0xfffff83f */

#ifndef OP_MASK
#define OP_MASK                         0xffffffff
#endif

/* These are the data structures we use to hold the instruction information */

extern const struct nios2_opcode nios2_builtin_opcodes[];
extern const int bfd_nios2_num_builtin_opcodes;
extern struct nios2_opcode *nios2_opcodes;
extern int bfd_nios2_num_opcodes;

/* These are the data structures used to hold the operand parsing information */
//extern const struct nios2_arg_parser nios2_arg_parsers[];
//extern struct nios2_arg_parser* nios2_arg_parsers;
//extern const int nios2_num_builtin_arg_parsers;
//extern int nios2_num_arg_parsers;

/* These are the data structures used to hold the register information */
extern const struct nios2_reg nios2_builtin_regs[];
extern struct nios2_reg *nios2_regs;
extern const int nios2_num_builtin_regs;
extern int nios2_num_regs;

/* Machine-independent macro for number of opcodes */

#define NUMOPCODES bfd_nios2_num_opcodes
#define NUMREGISTERS nios2_num_regs;

/* these are used in disassembly to get the correct register names */
#define NUMREGNAMES 32
#define NUMCTLREGNAMES 32
#define CTLREGBASE     42
#define COPROCREGBASE  83
#define NUMCOPROCREGNAMES 32


/* this is made extern so that the assembler can use it to find out
   what instruction caused an error */
extern const struct nios2_opcode *nios2_find_opcode_hash (unsigned long);

/* overflow message strings used in the assembler */
extern char *overflow_msgs[];

#endif // _NIOS2_H
