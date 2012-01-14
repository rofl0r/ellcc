# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH := $(shell uname -m)
XCFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCCINCDIR = $(shell $(XCC) -print-search-dirs | grep install: | sed  "s/install: //")include
XCFLAGS += -I$(XCCINCDIR)
DOFF = ../../../../../../libecc
XCFLAGS += -I$(DOFF)/usrinclude/$(ARCH)/$(OS)
XCFLAGS += -I$(DOFF)/usrinclude/$(ARCH) 
XCFLAGS += -I$(DOFF)/usrinclude/$(OS)
XCFLAGS += -I$(DOFF)/usrinclude
XCFLAGS += -I$(DOFF)/usrinclude/gcc
OSLIBDIR = $(DOFF)/lib/$(XCC)/$(OS)
XLDFLAGS += -nostdlib $(OSLIBDIR)/crt0.o $(OSLIBDIR)/crtbegin.o
XLDEXTRA = -L$(OSLIBDIR) -L$(DOFF)/lib/$(XCC) -lm -lc -lcompiler-rt $(OSLIBDIR)/crtend.o
EXE = ./
