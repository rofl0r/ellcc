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
	  $(MAKE) XCC=$(XCC) PROG=$@ VPATH=../$(DIRPATH)/$@ CFLAGS="$(CFLAGS)" \
	    LDFLAGS="$(LDFLAGS)" LDEXTRA="$(LDEXTRA)" \
	    TARGET=$(TARGET) ARCH=$(ARCH) \
	    -C $@ $@ -f ../$(DIRPATH)/$@/Makefile ; \
	else \
	  $(MAKE) XCC=$(XCC) PROG=$@ VPATH=../$(DIRPATH)/$@ CFLAGS="$(CFLAGS)" \
	    LDFLAGS="$(LDFLAGS)" LDEXTRA="$(LDEXTRA)" \
	    TARGET=$(TARGET) ARCH=$(ARCH) \
	    -C $@ $@ -f ../$(SRCPATH)/Makefile ; \
	fi

install: $(PROGRAMS:%=%.install)

$(PROGRAMS:%=%.install):
	@echo install $(@:%.install=%)
	@mkdir -p $(@:%.install=%)
	@if [ -e ../$(DIRPATH)/$(@:%.install=%)/Makefile ] ; then \
	  $(MAKE) PROG=$@ VPATH=../$(DIRPATH)/$@ CFLAGS="$(CFLAGS)" \
	    -C $(@:%.install=%) \
	    install -f ../$(DIRPATH)/$(@:%.install=%)/Makefile ; \
	else \
	  $(MAKE) VPATH=$(DIRPATH)/$(@:%.install=%) -C $(@:%.install=%) \
	    install -f ../$(SRCPATH)/Makefile ; \
	fi

clean: $(PROGRAMS:%=%.clean)

$(PROGRAMS:%=%.clean):
	@echo clean $(@:%.clean=%)
	@mkdir -p $(@:%.clean=%)
	@if [ -e ../$(DIRPATH)/$(@:%.clean=%)/Makefile ] ; then \
	  $(MAKE) PROG=$@ VPATH=../$(DIRPATH)/$@ CFLAGS="$(CFLAGS)" \
	    -C $(@:%.clean=%) \
	    clean -f ../$(DIRPATH)/$(@:%.clean=%)/Makefile ; \
	else \
	  $(MAKE) -C $(@:%.clean=%) \
	    clean -f ../$(SRCPATH)/Makefile ; \
	fi

veryclean: $(PROGRAMS:%=%.veryclean)

$(PROGRAMS:%=%.veryclean):
	@echo veryclean $(@:%.veryclean=%)
	@rm -fr $(@:%.veryclean=%)
