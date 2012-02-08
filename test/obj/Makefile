# Build subdirectories.

# Define a subset for a quick sanity build.
# If SUBDIRS is not defined, all subdirectories with a Makefile will be built.
# SUBDIRS = arm armv7 armv7sf gcc gcc.cov i386 microblaze mips mips32r2 mips32r2elsf \
            mips32r2sf mipsel nios2 pic32 ppc ppc64 sparc x86_64
#SUBDIRS = gcc.cov x86_64 mips
# Define directories to ignore.
# RICH: http://ellcc.org/bugzilla/show_bug.cgi?id=46
IGNORE = ppc64
# Define subdirectories to ignore.
#SUBIGNORE =

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
