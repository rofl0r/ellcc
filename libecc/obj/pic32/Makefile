# Build subdirectories.

# Define a subset for a quick sanity build
# SUBDIRS = 
# Define directories to ignore.
# IGNORE = 
# Define subdirectories to ignore.
# SUBIGNORE = 

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
