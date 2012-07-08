# Build ELLCC.

# Get the names of the subdirectories.
SUBDIRS := llvm-build gnu libecc

all install clean veryclean check: llvm-build gnu/gnu-build
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1 ; \
	done

llvm-build gnu/gnu-build:
	./configure

.PHONY: release
release:
	mkdir -p release
	make -C libecc veryclean
	tar --exclude "*.svn*" --exclude "*/test/*" -cvpz -frelease/ellcc.tgz bin libecc

