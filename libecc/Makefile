# Build the ELLCC libraries.

# RICH: TODO: Check out the warnings.
PWD := $(shell pwd)
ELLCC := $(PWD)/..
OBJ := $(PWD)/musl-build
CFLAGS := -MP -MD -g -Werror -Qunused-arguments \
          -Wno-unneeded-internal-declaration -Wno-cast-align \
	  -Wno-incompatible-pointer-types -Wno-string-plus-int \
	  -Wno-pointer-sign -Wno-array-bounds -Wno-dangling-else
CFLAGS.arm := -mcpu=cortex-a9 -mfpu=neon
CFLAGS.armeb := -mcpu=cortex-a9 -mfpu=neon
CFLAGS.i386 :=
CFLAGS.microblaze :=
CFLAGS.mips := -mcpu=mips32r2
CFLAGS.ppc := -mcpu=e500
CFLAGS.ppc64 := -mcpu=e500
CFLAGS.x86_64 :=

.PHONY: libecc
libecc: arm \
        armeb \
	i386 \
	mips \
	ppc \
	ppc64 \
	x86_64 \
	# microblaze

arm armeb i386 mips ppc ppc64 x86_64 microblaze:
	@echo Making libecc for $@ in musl-build/$@
	mkdir -p $(OBJ)/$@
	cd src/musl ; \
	echo $@ >musl.new ; \
	(cmp musl.new musl.last || (make clean ; \
	  export CC="$(ELLCC)/bin/ecc" ; \
	    ./configure --prefix=$(OBJ)/$@ \
	      --build=$@ \
	      --syslibdir=$(OBJ)/$@ \
	      --exec-prefix=$(OBJ)/$@ \
	      --enable-warnings \
	      --enable-debug \
              --disable-shared \
	      CFLAGS="-target $@-ellcc-linux $(CFLAGS.$@) $(CFLAGS)" \
	      || exit 1)) ; \
	  mv musl.new musl.last ; \
	  cd ../..
	make -C src/musl || exit 1
	make -C src/musl install || exit 1

clean:
	make -C src/musl distclean

veryclean: clean
	rm -fr src/musl/musl.last $(OBJ)

install:
	@cd musl-build ; \
	for target in `echo *` ; do \
	  echo Installing libecc for $$target ; \
	  mkdir -p ../include/linux ; \
	  cp -r $$target/include/* ../include/linux ; \
	  mkdir -p ../include/$$target/linux ; \
	  rm -fr ../include/$$target/linux/bits ; \
	  mv ../include/linux/bits ../include/$$target/linux/bits ; \
	  mkdir -p ../lib/$$target/linux ; \
	  cp $$target/lib/*.a $$target/lib/*.o ../lib/$$target/linux ; \
	done

