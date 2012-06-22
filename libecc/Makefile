# Build the ELLCC libraries.

# RICH: TODO: Check out the warnings.
PWD := $(shell pwd)
ELLCC := $(PWD)/..
OBJ := $(PWD)/musl-build
TARGETS := armeb x86_64 arm i386 mips ppc ppc64 # microblaze
CFLAGS.arm := -mcpu=cortex-a9 -mfpu=neon
CFLAGS.armeb := -mcpu=cortex-a9 -mfpu=neon
CFLAGS.i386 :=
CFLAGS.microblaze :=
CFLAGS.mips := -mcpu=mips32r2
CFLAGS.ppc := -mcpu=e500
CFLAGS.ppc64 := -mcpu=e500
CFLAGS.x86_64 :=
.PHONY: libecc
libecc: arm armeb i386 mips ppc ppc64 x86_64 # microblaze

.PHONY: arm
	@for target in $(TARGETS) ; do \
	  echo Making libecc for $$target $@ in musl-build/$$target ; \
	  mkdir -p $(OBJ)/$$target ; \
	  cd src/musl ; \
	  make clean ; \
	  make echo "TC = $(CFLAGS.$$target)" ; \
	  export CC="$(ELLCC)/bin/ecc" ; \
	  ./configure --prefix=$(OBJ)/$$target \
	    --build=$$target \
	    --syslibdir=$(OBJ)/$$target \
	    --exec-prefix=$(OBJ)/$$target \
	    --enable-warnings \
	    --enable-debug \
            --disable-shared \
	    CFLAGS="-target $$target-ellcc-linux $(CFLAGS.$$target) -MP -MD -g -Werror -Qunused-arguments -Wno-unneeded-internal-declaration -Wno-cast-align -Wno-incompatible-pointer-types -Wno-string-plus-int -Wno-pointer-sign -Wno-array-bounds" \
	    || exit 1 ; \
	  make || exit 1 ; \
	  make install || exit 1 ; \
	  cd ../.. ; \
	done
