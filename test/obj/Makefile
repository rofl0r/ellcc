# Build subdirectories.
# If SUBDIRS is not defined, all subdirectories with a Makefile will be built.
# SUBDIRS = arm i386 microblaze mips mipsel nios2 ppc ppc64 sparc x86_64
# Define directories to ignore.
IGNORE = ppc ppc64 armeb
# Define subdirectories to ignore.
SUBIGNORE =

# Find Makefiles
MAKEFILES := $(shell echo */Makefile)

# Get the names of the subdirectories.
SUBDIRS ?= $(patsubst %/,%,$(dir $(MAKEFILES)))
SUBDIRS := $(filter-out $(IGNORE), $(SUBDIRS))

all install clean veryclean check:
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) IGNORE=$(SUBIGNORE) -C $$dir $@ || exit 1; \
	done
