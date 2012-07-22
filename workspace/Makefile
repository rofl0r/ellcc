# Build subdirectories.
SUBDIRS = obj
# Define directories to ignore.
IGNORE = 

# Find Makefiles
MAKEFILES := $(shell echo */Makefile)

# Get the names of the subdirectories.
SUBDIRS ?= $(patsubst %/,%,$(dir $(MAKEFILES)))
SUBDIRS := $(filter-out $(IGNORE), $(SUBDIRS))

all install clean veryclean check:
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1; \
	done
