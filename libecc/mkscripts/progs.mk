# Build rules for a set of OS specific programs.
LEVEL=../..
OS = $(shell basename `cd ../; pwd`)
OSDIR = /$(OS)

# The target processor.
TARGET = $(shell basename `cd ../..; pwd`)
# Check for code coverage.
COV := $(shell basename $(TARGET) .cov)
ifneq ($(TARGET),$(COV))
    TARGET := $(COV)
    XCFLAGS += -fprofile-arcs -ftest-coverage
    XCXXFLAGS += -fprofile-arcs -ftest-coverage
    XLDFLAGS += -coverage
endif

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

# Build the library.
SRCPATH := $(LEVEL)/../../src
DIRPATH = $(SRCPATH)/$(DIR)
# The programs to build.
PROGRAMS = $(shell cd $(DIRPATH); echo *)

all: $(PROGRAMS)

.PHONY: $(PROGRAMS)

$(PROGRAMS):
	@echo build $@
	@mkdir -p $@
	@if [ -e $(DIRPATH)/$@/Makefile ] ; then \
	  $(MAKE) XCC=$(XCC) PROG=$@ VPATH=../$(DIRPATH)/$@ XCFLAGS="$(XCFLAGS)" \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    OS=$(OS) TARGET=$(TARGET) ARCH=$(ARCH) \
	    -C $@ $@ -f ../$(DIRPATH)/$@/Makefile ; \
	else \
	  $(MAKE) XCC=$(XCC) PROG=$@ VPATH=../$(DIRPATH)/$@ XCFLAGS="$(XCFLAGS)" \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    OS=$(OS) TARGET=$(TARGET) ARCH=$(ARCH) \
	    -C $@ $@ -f $(ELLCC)/libecc/mkscripts/prog.mk ; \
	fi

install: $(PROGRAMS:%=%.install)

$(PROGRAMS:%=%.install):
	@echo install $(@:%.install=%)
	@mkdir -p $(@:%.install=%)
	@if [ -e ../$(DIRPATH)/$(@:%.install=%)/Makefile ] ; then \
	  $(MAKE) PROG=$@ VPATH=../$(DIRPATH)/$@ XCFLAGS="$(XCFLAGS)" \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    -C $(@:%.install=%) \
	    install -f ../$(DIRPATH)/$(@:%.install=%)/Makefile ; \
	else \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.install=%) -C $(@:%.install=%) \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    install -f $(ELLCC)/libecc/mkscripts/prog.mk ; \
	fi

clean: $(PROGRAMS:%=%.clean)

$(PROGRAMS:%=%.clean):
	@echo clean $(@:%.clean=%)
	@mkdir -p $(@:%.clean=%)
	@if [ -e ../$(DIRPATH)/$(@:%.clean=%)/Makefile ] ; then \
	  $(MAKE) PROG=$@ VPATH=../$(DIRPATH)/$@ XCFLAGS="$(XCFLAGS)" \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    -C $(@:%.clean=%) \
	    clean -f ../$(DIRPATH)/$(@:%.clean=%)/Makefile ; \
	else \
	  $(MAKE) -C $(@:%.clean=%) \
	    ELLCC="$(ELLCC)" XLDFLAGS="$(XLDFLAGS)" XLDEXTRA="$(XLDEXTRA)" \
	    clean -f $(ELLCC)/libecc/mkscripts/prog.mk ; \
	fi

veryclean: $(PROGRAMS:%=%.veryclean)

$(PROGRAMS:%=%.veryclean):
	@echo veryclean $(@:%.veryclean=%)
	@rm -fr $(@:%.veryclean=%)
