/* nios2-opc.c -- Altera New Jersey opcode list.

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

#include <stdio.h>
#include "opcode/nios2.h"

/* Register string table */

const struct nios2_reg nios2_builtin_regs[] = {
  {"zero", 0},
  {"at", 1},			// assembler temporary
  {"r2", 2},
  {"r3", 3},
  {"r4", 4},
  {"r5", 5},
  {"r6", 6},
  {"r7", 7},
  {"r8", 8},
  {"r9", 9},
  {"r10", 10},
  {"r11", 11},
  {"r12", 12},
  {"r13", 13},
  {"r14", 14},
  {"r15", 15},
  {"r16", 16},
  {"r17", 17},
  {"r18", 18},
  {"r19", 19},
  {"r20", 20},
  {"r21", 21},
  {"r22", 22},
  {"r23", 23},
  {"et", 24},
  {"bt", 25},
  {"gp", 26},			/* global pointer */
  {"sp", 27},			/* stack pointer */
  {"fp", 28},			/* frame pointer */
  {"ea", 29},			/* exception return address */
  {"ba", 30},			/* breakpoint return address */
  {"ra", 31},			/* return address */

  /* alternative names for special registers */
  {"r0", 0},
  {"r1", 1},
  {"r24", 24},
  {"r25", 25},
  {"r26", 26},
  {"r27", 27},
  {"r28", 28},
  {"r29", 29},
  {"r30", 30},
  {"r31", 31},

  /* control register names */
  {"status", 0},
  {"estatus", 1},
  {"bstatus", 2},
  {"ienable", 3},
  {"ipending", 4},
  {"cpuid", 5},
  {"ctl6", 6},
  {"ctl7", 7},
  {"pteaddr", 8},
  {"tlbacc", 9},
  {"tlbmisc", 10},
  {"fstatus", 11},
  {"ctl12", 12},
  {"ctl13", 13},
  {"ctl14", 14},
  {"ctl15", 15},
  {"ctl16", 16},
  {"ctl17", 17},
  {"ctl18", 18},
  {"ctl19", 19},
  {"ctl20", 20},
  {"ctl21", 21},
  {"ctl22", 22},
  {"ctl23", 23},
  {"ctl24", 24},
  {"ctl25", 25},
  {"ctl26", 26},
  {"ctl27", 27},
  {"ctl28", 28},
  {"ctl29", 29},
  {"ctl30", 30},
  {"ctl31", 31},

  /* alternative names for special control registers */
  {"ctl0", 0},
  {"ctl1", 1},
  {"ctl2", 2},
  {"ctl3", 3},
  {"ctl4", 4},
  {"ctl5", 5},
  {"ctl8", 8},
  {"ctl9", 9},
  {"ctl10", 10},
  {"ctl11", 11},

  /* coprocessor register names */
  {"c0", 0},
  {"c1", 1},
  {"c2", 2},
  {"c3", 3},
  {"c4", 4},
  {"c5", 5},
  {"c6", 6},
  {"c7", 7},
  {"c8", 8},
  {"c9", 9},
  {"c10", 10},
  {"c11", 11},
  {"c12", 12},
  {"c13", 13},
  {"c14", 14},
  {"c15", 15},
  {"c16", 16},
  {"c17", 17},
  {"c18", 18},
  {"c19", 19},
  {"c20", 20},
  {"c21", 21},
  {"c22", 22},
  {"c23", 23},
  {"c24", 24},
  {"c25", 25},
  {"c26", 26},
  {"c27", 27},
  {"c28", 28},
  {"c29", 29},
  {"c30", 30},
  {"c31", 31},
};

#define NIOS2_NUM_REGS \
       ((sizeof nios2_builtin_regs) / (sizeof (nios2_builtin_regs[0])))
const int nios2_num_builtin_regs = NIOS2_NUM_REGS;

/* const removed from the following to allow for dynamic extensions to the
 * built-in instruction set. */
struct nios2_reg *nios2_regs = (struct nios2_reg *) nios2_builtin_regs;
int nios2_num_regs = NIOS2_NUM_REGS;
#undef NIOS2_NUM_REGS

/* overflow message string templates */

char *overflow_msgs[] = {
  "call target address 0x%08x out of range 0x%08x to 0x%08x",
  "branch offset %d out of range %d to %d",
  "%s offset %d out of range %d to %d",
  "immediate value %d out of range %d to %d",
  "immediate value %u out of range %u to %u",
  "immediate value %u out of range %u to %u",
  "custom instruction opcode %u out of range %u to %u",
};



/*--------------------------------------------------------------------------------
   This is the opcode table used by the New Jersey GNU as, disassembler and GDB
  --------------------------------------------------------------------------------*/

/*
       The following letters can appear in the args field of the nios2_opcode
       structure:

       c - a 5-bit control register index or break opcode
       d - a 5-bit destination register index
       s - a 5-bit left source register index
       t - a 5-bit right source register index
       i - a 16-bit signed immediate
       u - a 16-bit unsigned immediate

    j - a 5-bit unsigned immediate
       k - a 6-bit unsigned immediate
       l - an 8-bit unsigned immediate
       m - a 26-bit unsigned immediate
*/

/* *INDENT-OFF* */
/* FIXME: Re-format for GNU standards */
const struct nios2_opcode nios2_builtin_opcodes[] =
{
   /* name,             args,           args_test     num_args,          match,                  mask,                                   pinfo */
       {"add",          "d,s,t",        "d,s,t,E",     3,         OP_MATCH_ADD,           OP_MASK_ROPX | OP_MASK_ROP,      	      0,									no_overflow },
       {"addi",         "t,s,i",        "t,s,i,E",     3,         OP_MATCH_ADDI,          OP_MASK_IOP,                            NIOS2_INSN_ADDI,						signed_immed16_overflow },
       {"subi",         "t,s,i",        "t,s,i,E",     3,         OP_MATCH_ADDI,          OP_MASK_IOP,                            NIOS2_INSN_MACRO,						signed_immed16_overflow },
       {"and",          "d,s,t",        "d,s,t,E",     3,         OP_MATCH_AND,           OP_MASK_ROPX | OP_MASK_ROP,			  0,									no_overflow },
       {"andhi",        "t,s,u",        "t,s,u,E",     3,         OP_MATCH_ANDHI,         OP_MASK_IOP,                            0,									unsigned_immed16_overflow },
       {"andi",         "t,s,u",        "t,s,u,E",     3,         OP_MATCH_ANDI,          OP_MASK_IOP,                            NIOS2_INSN_ANDI,						unsigned_immed16_overflow },
       {"beq",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BEQ,           OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,  					branch_target_overflow },
       {"bge",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BGE,           OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,					branch_target_overflow },
       {"bgeu",         "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BGEU,          OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,					branch_target_overflow },
       {"bgt",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BLT,           OP_MASK_IOP,                            NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,		branch_target_overflow },
       {"bgtu",         "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BLTU,          OP_MASK_IOP,                            NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,		branch_target_overflow },
       {"ble",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BGE,           OP_MASK_IOP,                            NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,		branch_target_overflow },
       {"bleu",         "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BGEU,          OP_MASK_IOP,                            NIOS2_INSN_MACRO|NIOS2_INSN_CBRANCH,		branch_target_overflow },
       {"blt",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BLT,           OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,					branch_target_overflow },
       {"bltu",         "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BLTU,          OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,					branch_target_overflow },
       {"bne",          "s,t,o",        "s,t,o,E",     3,         OP_MATCH_BNE,           OP_MASK_IOP,                            NIOS2_INSN_CBRANCH,					branch_target_overflow },
       {"br",           "o",            "o,E",         1,         OP_MATCH_BR,            OP_MASK_IOP,                            NIOS2_INSN_UBRANCH,					branch_target_overflow },
       {"break",        "b",            "b,E",         1,         OP_MATCH_BREAK,         OP_MASK_BREAK,                          0,				 					no_overflow },
       {"bret",         "",             "E",           0,         OP_MATCH_BRET,          OP_MASK,                                0, 									no_overflow },
       {"flushd",       "i(s)",       	"i(s)E",       2,         OP_MATCH_FLUSHD,        OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"flushda",      "i(s)",       	"i(s)E",       2,         OP_MATCH_FLUSHDA,       OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"flushi",       "s",       		"s,E",	       1,         OP_MATCH_FLUSHI,        OP_MASK_FLUSHI,			              0,				 					no_overflow },
       {"flushp",       "",       		"E",	       0,         OP_MATCH_FLUSHP,        OP_MASK,	                              0,				 					no_overflow },
 	   {"initd",       "i(s)",       	"i(s)E",       2,         OP_MATCH_INITD,         OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"initi",       "s",       		"s,E",	       1,         OP_MATCH_INITI,         OP_MASK_INITI,			              0,				 					no_overflow },
       {"call",         "m",            "m,E",         1,         OP_MATCH_CALL,          OP_MASK_IOP,                            NIOS2_INSN_CALL,						call_target_overflow },
       {"callr",        "s",            "s,E",         1,         OP_MATCH_CALLR,         OP_MASK_CALLR,                          0, 									no_overflow },
       {"cmpeq",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPEQ,         OP_MASK_ROPX | OP_MASK_ROP, 		      0,									no_overflow },
       {"cmpeqi",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPEQI,        OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"cmpge",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPGE,         OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"cmpgei",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPGEI,        OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"cmpgeu",       "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPGEU,        OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"cmpgeui",      "t,s,u",        "t,s,u,E",     3,         OP_MATCH_CMPGEUI,       OP_MASK_IOP,                            0,									unsigned_immed16_overflow },
       {"cmpgt",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPLT,         OP_MASK_ROPX | OP_MASK_ROP,             NIOS2_INSN_MACRO, 					no_overflow },
       {"cmpgti",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPGEI,        OP_MASK_IOP,                            NIOS2_INSN_MACRO,						signed_immed16_overflow },
       {"cmpgtu",       "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPLTU,        OP_MASK_ROPX | OP_MASK_ROP,             NIOS2_INSN_MACRO, 					no_overflow },
       {"cmpgtui",      "t,s,u",        "t,s,u,E",     3,         OP_MATCH_CMPGEUI,       OP_MASK_IOP,                            NIOS2_INSN_MACRO,						unsigned_immed16_overflow },
       {"cmple",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPGE,         OP_MASK_ROPX | OP_MASK_ROP,             NIOS2_INSN_MACRO, 					no_overflow },
       {"cmplei",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPLTI,        OP_MASK_IOP,                            NIOS2_INSN_MACRO,						signed_immed16_overflow },
       {"cmpleu",       "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPGEU,        OP_MASK_ROPX | OP_MASK_ROP,             NIOS2_INSN_MACRO, 					no_overflow },
       {"cmpleui",      "t,s,u",        "t,s,u,E",     3,         OP_MATCH_CMPLTUI,       OP_MASK_IOP,                            NIOS2_INSN_MACRO,						unsigned_immed16_overflow },
       {"cmplt",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPLT,         OP_MASK_ROPX | OP_MASK_ROP,             0,									no_overflow },
       {"cmplti",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPLTI,        OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"cmpltu",       "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPLTU,        OP_MASK_ROPX | OP_MASK_ROP,             0,									no_overflow },
       {"cmpltui",      "t,s,u",        "t,s,u,E",     3,         OP_MATCH_CMPLTUI,       OP_MASK_IOP,                            0,									unsigned_immed16_overflow },
       {"cmpne",        "d,s,t",        "d,s,t,E",     3,         OP_MATCH_CMPNE,         OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"cmpnei",       "t,s,i",        "t,s,i,E",     3,         OP_MATCH_CMPNEI,        OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"div",          "d,s,t",        "d,s,t,E",     3,         OP_MATCH_DIV,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"divu",         "d,s,t",        "d,s,t,E",     3,         OP_MATCH_DIVU,          OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"jmp",          "s",            "s,E",         1,         OP_MATCH_JMP,           OP_MASK_JMP,                            0, 									no_overflow },
       {"jmpi",         "m",            "m,E",         1,         OP_MATCH_JMPI,          OP_MASK_IOP,                            0,						                        no_overflow },
       {"ldb",          "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDB,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldbio",        "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDBIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldbu",         "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDBU,          OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldbuio",       "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDBUIO,        OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldh",          "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDH,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldhio",        "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDHIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldhu",         "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDHU,          OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldhuio",       "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDHUIO,        OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldw",          "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDW,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"ldwio",        "t,i(s)",       "t,i(s)E",     3,         OP_MATCH_LDWIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"mov",          "d,s",          "d,s,E",       2,         OP_MATCH_ADD,           OP_MASK_RRT|OP_MASK_ROPX|OP_MASK_ROP,   NIOS2_INSN_MACRO_MOV, 				no_overflow },
       {"movhi",        "t,u",          "t,u,E",       2,         OP_MATCH_ORHI,          OP_MASK_IRS|OP_MASK_IOP,                NIOS2_INSN_MACRO_MOVI,				unsigned_immed16_overflow },
       {"movui",        "t,u",          "t,u,E",       2,         OP_MATCH_ORI,           OP_MASK_IRS|OP_MASK_IOP,                NIOS2_INSN_MACRO_MOVI,				unsigned_immed16_overflow },
       {"movi",         "t,i",          "t,i,E",       2,         OP_MATCH_ADDI,          OP_MASK_IRS|OP_MASK_IOP,                NIOS2_INSN_MACRO_MOVI,				signed_immed16_overflow },
       /* movia expands to two instructions so there is no mask or match */
       {"movia",		"t,o",			"t,o,E",		2,		OP_MATCH_ORHI,          OP_MASK_IOP,							  NIOS2_INSN_MACRO_MOVIA,				no_overflow },
       {"mul",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_MUL,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"muli",         "t,s,i",        "t,s,i,E",      3,        OP_MATCH_MULI,          OP_MASK_IOP,                            0,									signed_immed16_overflow },
       {"mulxss",       "d,s,t",        "d,s,t,E",      3,        OP_MATCH_MULXSS,        OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"mulxsu",       "d,s,t",        "d,s,t,E",      3,        OP_MATCH_MULXSU,        OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"mulxuu",       "d,s,t",        "d,s,t,E",      3,        OP_MATCH_MULXUU,        OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"nextpc",       "d",            "d,E",          1,        OP_MATCH_NEXTPC,        OP_MASK_NEXTPC,                         0, 									no_overflow },
       {"nop",          "",             "E",            0,        OP_MATCH_ADD,           OP_MASK,                                NIOS2_INSN_MACRO_MOV,					no_overflow },
       {"nor",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_NOR,           OP_MASK_ROPX | OP_MASK_ROP,             0,									no_overflow },
       {"or",           "d,s,t",        "d,s,t,E",      3,        OP_MATCH_OR,            OP_MASK_ROPX | OP_MASK_ROP,             0,									no_overflow },
       {"orhi",         "t,s,u",        "t,s,u,E",      3,        OP_MATCH_ORHI,          OP_MASK_IOP,                            0,									unsigned_immed16_overflow },
       {"ori",          "t,s,u",        "t,s,u,E",      3,        OP_MATCH_ORI,           OP_MASK_IOP,                            NIOS2_INSN_ORI,						unsigned_immed16_overflow },
       {"rdctl",        "d,c",          "d,c,E",        2,        OP_MATCH_RDCTL,         OP_MASK_RDCTL,                          0, 									no_overflow },
       {"ret",          "",             "E",            0,        OP_MATCH_RET,           OP_MASK,                                0, 									no_overflow },
       {"rol",          "d,s,t",        "d,s,t,E",      3,      OP_MATCH_ROL,           OP_MASK_ROPX | OP_MASK_ROP,               0, 									no_overflow },
       {"roli",         "d,s,j",        "d,s,j,E",      3,        OP_MATCH_ROLI,          OP_MASK_ROLI,                           0, 									unsigned_immed5_overflow },
       {"ror",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_ROR,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"sll",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_SLL,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"slli",         "d,s,j",        "d,s,j,E",      3,        OP_MATCH_SLLI,          OP_MASK_SLLI,                           0, 									unsigned_immed5_overflow },
       {"sra",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_SRA,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"srai",         "d,s,j",        "d,s,j,E",      3,        OP_MATCH_SRAI,          OP_MASK_SRAI,                           0, 									unsigned_immed5_overflow },
       {"srl",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_SRL,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"srli",         "d,s,j",        "d,s,j,E",      3,        OP_MATCH_SRLI,          OP_MASK_SRLI,                           0, 									unsigned_immed5_overflow },
       {"stb",          "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STB,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"stbio",        "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STBIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"sth",          "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STH,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"sthio",        "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STHIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"stw",          "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STW,           OP_MASK_IOP,                            0,									address_offset_overflow },
       {"stwio",        "t,i(s)",       "t,i(s)E",      3,        OP_MATCH_STWIO,         OP_MASK_IOP,                            0,									address_offset_overflow },
       {"sub",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_SUB,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"sync",         "",             "E",            0,        OP_MATCH_SYNC,          OP_MASK_SYNC,                           0, 									no_overflow },
       {"trap",         "",             "E",            0,      	OP_MATCH_TRAP,          OP_MASK_TRAP,                         0, 									no_overflow },
       {"eret",         "",             "E",            0,      OP_MATCH_ERET,          OP_MASK,                                  0,									no_overflow },
       {"custom",          "l,d,s,t",      "l,d,s,t,E",    4,        OP_MATCH_CUSTOM,           OP_MASK_ROP,                      0,	 								custom_opcode_overflow },
       {"wrctl",        "c,s",          "c,s,E",        2,        OP_MATCH_WRCTL,         OP_MASK_WRCTL,                          0, 									no_overflow },
       {"xor",          "d,s,t",        "d,s,t,E",      3,        OP_MATCH_XOR,           OP_MASK_ROPX | OP_MASK_ROP,             0, 									no_overflow },
       {"xorhi",        "t,s,u",        "t,s,u,E",      3,        OP_MATCH_XORHI,         OP_MASK_IOP,                            0,									unsigned_immed16_overflow },
       {"xori",         "t,s,u",        "t,s,u,E",      3,        OP_MATCH_XORI,          OP_MASK_IOP,                            NIOS2_INSN_XORI,						unsigned_immed16_overflow }
};
/* *INDENT-ON* */

#define NIOS2_NUM_OPCODES \
       ((sizeof nios2_builtin_opcodes) / (sizeof (nios2_builtin_opcodes[0])))
const int bfd_nios2_num_builtin_opcodes = NIOS2_NUM_OPCODES;

/* const removed from the following to allow for dynamic extensions to the
 * built-in instruction set. */
struct nios2_opcode *nios2_opcodes =
  (struct nios2_opcode *) nios2_builtin_opcodes;
int bfd_nios2_num_opcodes = NIOS2_NUM_OPCODES;
#undef NIOS2_NUM_OPCODES
