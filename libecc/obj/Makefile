# Build subdirectories.

# Define a subset for a quick sanity build.
# If SUBDIRS is not defined, all subdirectories with a Makefile will be built.
# SUBDIRS = arm i386 microblaze mips mipsel nios2 ppc ppc64 sparc x86_64
#SUBDIRS = gcc.cov x86_64 mips
# Define directories to ignore.
IGNORE = nios2 sparc microblaze
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

copy:
	@test -d ../src || (echo Not at the top level.; exit 1)
	@for dir in $(SUBDIRS) ; do \
	  cp Makefile $$dir; \
	done
