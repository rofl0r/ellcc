##===- lib/Target/Nios2/Makefile ----------------------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##
LEVEL = ../../..
LIBRARYNAME = LLVMNios2CodeGen
TARGET = Nios2

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = Nios2GenRegisterInfo.h.inc Nios2GenRegisterNames.inc \
		Nios2GenRegisterInfo.inc Nios2GenInstrNames.inc \
		Nios2GenInstrInfo.inc Nios2GenAsmWriter.inc \
		Nios2GenDAGISel.inc Nios2GenCallingConv.inc \
		Nios2GenSubtarget.inc

DIRS = AsmPrinter TargetInfo

include $(LEVEL)/Makefile.common

