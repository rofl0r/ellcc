# Makefile for toplevel ELLCC distribution

# just do the obvious recursive thing
all:
	$(MAKE) -C ellcc

check:
	$(MAKE) -C ellcc check

clean:
	$(MAKE) -C ellcc clean

distclean:
	$(MAKE) -C ellcc distclean

doc:
	$(MAKE) -C ellcc doc
