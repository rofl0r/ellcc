# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
ARCH := $(shell uname -m)
XCFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCCINCDIR = $(shell $(XCC) -print-search-dirs | grep install: | sed  "s/install: //")include
XCFLAGS += -I$(XCCINCDIR)
XCFLAGS += -I../../../../../../test/include
DOFF = ../../../../../../libecc
XCFLAGS += -I$(DOFF)/include/$(ARCH)/$(OS)
XCFLAGS += -I$(DOFF)/include/$(ARCH) 
XCFLAGS += -I$(DOFF)/include/$(OS)
XCFLAGS += -I$(DOFF)/include
XCFLAGS += -I$(DOFF)/include/gcc
OSLIBDIR = $(DOFF)/lib/$(XCC)/$(OS)
XLDFLAGS += -nostdlib $(OSLIBDIR)/crt1.o $(OSLIBDIR)/crtbegin.o
XLDEXTRA = -L$(OSLIBDIR) -L$(DOFF)/lib/$(XCC) -lm -lc -lcompiler-rt $(OSLIBDIR)/crtend.o
EXE = ./
