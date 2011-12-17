# The target processor.
TARGET = $(shell basename `cd $(LEVEL); pwd`)

# Determine the architecture.
ifneq ($(filter arm%, $(TARGET)),)
  ARCH := arm
else ifneq ($(filter i386%, $(TARGET)),)
  ARCH := i386
else ifneq ($(filter microblaze%, $(TARGET)),)
  ARCH := microblaze
else ifneq ($(filter mips%, $(TARGET)),)
  ARCH := mips
else ifneq ($(filter nios2%, $(TARGET)),)
  ARCH := nios2
else ifneq ($(filter ppc64%, $(TARGET)),)
  ARCH := ppc64
else ifneq ($(filter ppc%, $(TARGET)),)
  ARCH := ppc
else ifneq ($(filter sparc%, $(TARGET)),)
  ARCH := sparc
else ifneq ($(filter x86_64%, $(TARGET)),)
  ARCH := x86_64
endif
ARCH ?= $(TARGET)

# The base of the directory name.
DIR = $(shell basename `pwd`)

# The target program directory.
PROGDIR  = $(LEVEL)/../../bin/$(TARGET)$(OSDIR)/$(DIR)

ifeq ($(XCC),)
  # The build compiler.
  CC = ../$(LEVEL)/../../../bin/$(TARGET)-$(OS)-ecc
else
  CC = $(XCC)
endif

CFLAGS += -Werror -MD -MP -O1

ifdef CPU
    MCPU = -mcpu=$(CPU)
endif

ifdef FLOAT
    MFLOAT = -m$(FLOAT)-float
endif

.SUFFIXES: .c .S .o
.c.o:
	${CC} $(MCPU) $(MFLOAT) -c ${CFLAGS} $<
.S.o:
	${CC} $(MCPU) -c ${CFLAGS} $<


# Build the library.
SRCPATH := $(LEVEL)/../../src
DIRPATH = $(SRCPATH)/$(DIR)
# The programs to build.
PROGRAMS = $(shell cd $(DIRPATH); echo *)

all: $(PROGRAMS:%=%.all)

$(PROGRAMS:%=%.all):
	@echo build $(@:%.all=%)
	@mkdir -p $(@:%.all=%)
	@if [ -e $(DIRPATH)/$(@:%.all=%)/Makefile ] ; then \
	  $(MAKE) CC=$(CC) PROG=$(@:%.all=%) VPATH=$(DIRPATH)/$(@:%.all=%) \
	    CFLAGS="$(CFLAGS)" SRCS=$(shell cd $(DIRPATH)/$(@:%.all=%); echo *.c) \
	    -C $(@:%.all=%) $(@:%.all=%) -f $(DIRPATH)/$(@:%.all=%)/Makefile ; \
	else \
	  $(MAKE) CC=$(CC) PROG=$(@:%.all=%) VPATH=../$(DIRPATH)/$(@:%.all=%) \
	    CFLAGS="$(CFLAGS)" SRCS=$(shell cd $(DIRPATH)/$(@:%.all=%); echo *.c) \
	    -C $(@:%.all=%) $(@:%.all=%) -f ../$(SRCPATH)/Makefile ; \
	fi

install: $(PROGRAMS:%=%.install)

$(PROGRAMS:%=%.install):
	@echo install $(@:%.install=%)
	@mkdir -p $(@:%.install=%)
	@if [ -e $(DIRPATH)/$(@:%.install=%)/Makefile ] ; then \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.install=%) -C $(@:%.install=%) \
	    install -f $(DIRPATH)/$(@:%.install=%)/Makefile ; \
	else \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.install=%) -C $(@:%.install=%) \
	    install -f ../$(SRCPATH)/Makefile ; \
	fi

clean: $(PROGRAMS:%=%.clean)

$(PROGRAMS:%=%.clean):
	@echo clean $(@:%.clean=%)
	@mkdir -p $(@:%.clean=%)
	@if [ -e $(DIRPATH)/$(@:%.clean=%)/Makefile ] ; then \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.clean=%) -C $(@:%.clean=%) \
	    clean -f $(DIRPATH)/$(@:%.clean=%)/Makefile ; \
	else \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.clean=%) -C $(@:%.clean=%) \
	    clean -f ../$(SRCPATH)/Makefile ; \
	fi

veryclean: $(PROGRAMS:%=%.veryclean)

$(PROGRAMS:%=%.veryclean):
	@echo veryclean $(@:%.veryclean=%)
	@rm -fr $(@:%.veryclean=%)
