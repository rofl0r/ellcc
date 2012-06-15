# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
ECC  := $(shell cd ../../../../..; pwd)
TGT := ppc64
XCC = $(ECC)/bin/$(TGT)-linux-ecc
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH := $(shell uname -m)
XCFLAGS = -std=gnu99 -nostdinc -D_XOPEN_SOURCE=700 -D_GNU_SOURCE -D__host__
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCFLAGS += -I$(ECC)/test/include
DOFF := $(ECC)/libecc
XCFLAGS += -I$(ECC)/musl-build/$(TGT)/include
OSLIBDIR := $(ECC)/musl-build/$(TGT)/lib
XLDFLAGS += -static -nostdlib $(OSLIBDIR)/crt1.o $(OSLIBDIR)/crtbegin.o
XLDEXTRA = -L$(OSLIBDIR) -lm -lc -L$(ECC)/libecc/lib/$(TGT) -lcompiler-rt $(OSLIBDIR)/crtend.o
#EXE = ./
EXE = "$(ECC)/bin/qemu-$(TGT) "
