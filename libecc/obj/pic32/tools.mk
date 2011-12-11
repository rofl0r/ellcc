# This file can be configured to compile libecc with an external compiler.
# The name of the external compiler.
TOOL_DIR = /opt/microchip/mplabc32/v2.01
TOOL_BIN_DIR = $(TOOL_DIR)/bin
XCC = $(TOOL_BIN_DIR)/pic32-gcc -v
# The archive building program.
AR = $(TOOL_BIN_DIR)/pic32-ar
# ARCH should be one of arm, i386, microblaze, mips, nios2, ppc64, ppc, sparc, or x86_64.
ARCH = mips
CFLAGS = -std=gnu99 -nostdinc
# Get compiler defined headers (this assumes XCC has gcc compatable command line options).
XCCINCDIR = $(shell $(XCC) -print-search-dirs | grep install: | sed  "s/install: //")include
CFLAGS += -I$(XCCINCDIR)
CFLAGS += -I$(DOFF)/../../usrinclude
CFLAGS += -I$(DOFF)/../../usrinclude/$(ARCH)/$(OS)
CFLAGS += -I$(DOFF)/../../usrinclude/$(ARCH) 
CFLAGS += -I$(DOFF)/../../usrinclude/$(OS)
CFLAGS += -I$(TOOL_DIR)/pic32mx/include
