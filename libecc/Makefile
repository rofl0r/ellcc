# Build the ELLCC libraries.

# RICH: TODO: Check out the warnings.
PWD := $(shell pwd)
ELLCC := $(PWD)/..
LIBECC := $(ELLCC)/libecc
Configs := $(patsubst %.notyet,,$(shell cd $(LIBECC)/targets; echo *))
INCLUDES := $(foreach TARGET, $(Configs), $(LIBECC)/targets/$(TARGET)/setup.mk)
OS := linux
CFLAGS := -MP -MD -g -Werror -Qunused-arguments \
          -Wno-unneeded-internal-declaration -Wno-cast-align \
	  -Wno-incompatible-pointer-types -Wno-string-plus-int \
	  -Wno-pointer-sign -Wno-array-bounds -Wno-dangling-else
-include $(INCLUDES)

# Get the clang version.
VERSION := $(shell $(ELLCC)/bin/ecc -v |& grep version |& sed "s/.*version \([0-9]*\.[0-9]*\).*/\1/")

OBJ := $(PWD)/musl-build
install:header.install \
	$(Configs) \
	compiler-rt.install \
	other-libs

all:

$(Configs):
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
	      CFLAGS="$(CFLAGS.$@)" \
	      || exit 1)) ; \
	  mv musl.new musl.last ; \
	  cd ../..
	make -C src/musl || exit 1
	make -C src/musl install || exit 1
	make $@.musl.install
	make -C src/compiler-rt ellcc_linux-$@

clean:
	make -C src/musl distclean
	make -C src/compiler-rt clean
	make -C obj clean
	rm -fr src/musl/musl.last

veryclean: clean
	make -C obj veryclean
	rm -fr $(OBJ)

install:

header.install:
	@echo Installing ecc header files for version $(VERSION) ; \
	rm -fr clang ; \
	mkdir -p clang ; \
	cp ../lib/clang/$(VERSION)/include/* clang

musl.install:
	@cd musl-build ; \
	for target in `echo *` ; do \
	  make -C .. $$target.musl.install ; \
	done

%.musl.install:
	@cd musl-build ; \
	echo Installing libecc for $* ; \
	mkdir -p ../include/linux ; \
	cp -r $*/include/* ../include/linux ; \
	mkdir -p ../include/$*/linux ; \
	rm -fr ../include/$*/linux/bits ; \
	mv ../include/linux/bits ../include/$*/linux/bits ; \
	mkdir -p ../lib/$*/linux ; \
	cp $*/lib/*.a $*/lib/*.o ../lib/$*/linux

compiler-rt.install:
	cd src/compiler-rt/ellcc_linux ; \
	for target in `echo *` ; do \
	  echo Installing libcompiler-rt for $$target ; \
	  mkdir -p ../../../lib/$$target/linux ; \
	  cp $$target/*.a ../../../lib/$$target/linux ; \
	done

other-libs:
	mkdir -p include/c++ ; \
	cp src/c++/libcxx/include/* include/c++ ; \
	mkdir -p include/c++/ext ; \
	cp src/c++/libcxx/include/ext/* include/c++/ext ; \
	cp clang/unwind.h include/c++
	cp src/c++/libcxxabi/include/* include/c++ ; \
	make -C obj install
