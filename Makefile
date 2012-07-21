VERSION=0.0.0
# Build ELLCC.

# Get the names of the subdirectories.
SUBDIRS := llvm-build gnu libecc

all install clean veryclean check:: llvm-build gnu/gnu-build
	@for dir in $(SUBDIRS) ; do \
	  echo Making $@ in $$dir ; \
	  $(MAKE) -C $$dir $@ || exit 1 ; \
	done

install:: docs

llvm-build gnu/gnu-build:
	./configure

.PHONY: release
release:
	rm -fr ellcc-$(VERSION)
	mkdir -p ellcc-$(VERSION)
	make -C libecc veryclean
	tar --exclude "*.svn*" --exclude "*/test/*" -cvp -f- bin libecc | (cd ellcc-$(VERSION); tar xfp -)
	(cd ellcc-$(VERSION); tree -T "ELLCC Release Directory Tree" -H ellcc --nolinks > ../tree.html)
	tar -cvpz -fellcc-$(VERSION)-linux-x86_64.tgz ellcc-$(VERSION)

.PHONY: docs
docs:
	cp -r ./lib/share/doc/ld.html \
	./lib/share/doc/gdb \
	./lib/share/doc/binutils.html \
	./lib/share/doc/as.html \
	./share/doc/qemu \
	libecc/doc
