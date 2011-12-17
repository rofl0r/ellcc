# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH := $(shell uname -m)
CFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCCINCDIR = $(shell $(XCC) -print-search-dirs | grep install: | sed  "s/install: //")include
CFLAGS += -I$(XCCINCDIR)
DOFF = ../../../../../../libecc
CFLAGS += -I$(DOFF)/usrinclude/$(ARCH)/$(OS)
CFLAGS += -I$(DOFF)/usrinclude/$(ARCH) 
CFLAGS += -I$(DOFF)/usrinclude/$(OS)
CFLAGS += -I$(DOFF)/usrinclude
OSLIBDIR = $(DOFF)/lib/$(XCC)/$(OS)
LDFLAGS += -nostdlib $(OSLIBDIR)/crt0.o $(OSLIBDIR)/crtbegin.o
LDEXTRA = -L$(OSLIBDIR) -L$(DOFF)/lib/$(XCC) -lc -lcompiler-rt $(OSLIBDIR)/crtend.o
