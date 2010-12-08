# Build subdirectories.

# Find Makefiles
MAKEFILES := $(shell echo */Makefile)

# Get the names of the subdirectories.
SUBDIRS := $(dir $(MAKEFILES))

all install clean veryclean:
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1; \
	done

copy:
	@for dir in $(SUBDIRS) ; do \
	  cp Makefile $$dir; \
	done
