# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
XCC = gcc
# The archive building program.
AR = ar
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH = x86_64
CFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCCINCDIR = $(shell $(XCC) -print-search-dirs | grep install: | sed  "s/install: //")include
CFLAGS += -I$(XCCINCDIR)
CFLAGS += -I$(DOFF)/../../usrinclude
CFLAGS += -I$(DOFF)/../../usrinclude/$(ARCH)/$(OS)
CFLAGS += -I$(DOFF)/../../usrinclude/$(ARCH) 
CFLAGS += -I$(DOFF)/../../usrinclude/$(OS)
