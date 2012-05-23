# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
ECC  := $(shell cd ../../../../..; pwd)
XCC = $(ECC)/bin/x86_64-linux-ecc
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH := $(shell uname -m)
XCFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCFLAGS += -I$(ECC)/test/include
DOFF := $(ECC)/libecc
XCFLAGS += -I$(DOFF)/include
XCFLAGS += -I$(ECC)/musl-build/x86_64/include
OSLIBDIR := $(ECC)/musl-build/x86_64/lib
XLDFLAGS += -static -nostdlib $(OSLIBDIR)/crt1.o $(OSLIBDIR)/crti.o
XLDEXTRA = -L$(OSLIBDIR) $(OSLIBDIR)/crtn.o -lm -lc -L$(ECC)/libecc/lib/x86_64 -lcompiler-rt
EXE = ./
