# Build ELLCC.

# Get the names of the subdirectories.
SUBDIRS := llvm-build gnu

all install clean veryclean check: llvm-build gnu/gnu-build
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1; \
	done

llvm-build gnu/gnu-build:
	./configure
