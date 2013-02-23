# Build ELLCC.

# Get the names of the subdirectories.

SUBDIRS := gnu-build/binutils gnu-build/gdb  src/qemu \
	   gnu-build/binutils/arm-elf        gnu-build/binutils/ppc64-elf \
	   gnu-build/binutils/i386-elf       gnu-build/binutils/ppc-elf \
	   gnu-build/binutils/microblaze-elf gnu-build/binutils/sparc-elf \
	   gnu-build/binutils/mips-elf       gnu-build/binutils/x86_64-elf

all install clean veryclean check:: gnu-build
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1; \
	done

DOCDIRS := gnu-build/binutils gnu-build/gdb

install::
	@for dir in $(DOCDIRS) ; do \
	  echo Making install-html in $$dir ; \
	  $(MAKE) -C $$dir install-html || exit 1; \
	done

install::
	mkdir -p ../libecc/ldscripts/linux
	cp gnu-build/binutils/ld/ldscripts/* ../libecc/ldscripts/linux/

gnu-build:
	./configure
