# Build ELLCC.

# Get the names of the subdirectories.
SUBDIRS := llvm-build gnu

all install clean veryclean check: llvm-build gnu/gnu-build
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1 ; \
	done

llvm-build gnu/gnu-build:
	./configure

# RICH: TODO: Check out the warnings.
PWD := $(shell pwd)
TARGETS := x86_64 arm i386 # microblaze mips ppc ppc64
.PHONY: musl
musl:
	@for target in $(TARGETS) ; do \
	  echo Making musl for $$target $@ in musl-build/$$target ; \
	  mkdir -p musl-build/$$target ; \
	  cd musl ; \
	  make clean ; \
	  export CC="$(PWD)/bin/$$target-linux-ecc" ; \
	  ./configure --prefix=$(PWD)/musl-build/$$target \
	    --build=$$target \
	    --syslibdir=$(PWD)/musl-build/$$target \
	    --exec-prefix=$(PWD)/musl-build/$$target \
	    --enable-warnings \
	    CFLAGS="-g -Werror -Qunused-arguments -Wno-unneeded-internal-declaration -Wno-cast-align -Wno-incompatible-pointer-types -Wno-string-plus-int -Wno-pointer-sign -Wno-array-bounds" \
	    || exit 1 ; \
	  make || exit 1 ; \
	  make install || exit 1 ; \
	  cd .. ; \
	done
