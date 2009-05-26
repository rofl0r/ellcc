# Makefile for toplevel elsa distribution

# just do the obvious recursive thing
all:
	$(MAKE) -C smbase
	$(MAKE) -C basic
	$(MAKE) -C lex
	$(MAKE) -C ast
	$(MAKE) -C elkhound
	$(MAKE) -C elsa
	$(MAKE) -C ellcc

check:
	$(MAKE) -C smbase check
	$(MAKE) -C ast check
	$(MAKE) -C elkhound check
	$(MAKE) -C elsa check
	$(MAKE) -C ellcc check

clean:
	$(MAKE) -C smbase clean
	$(MAKE) -C basic clean
	$(MAKE) -C lex clean
	$(MAKE) -C ast clean
	$(MAKE) -C elkhound clean
	$(MAKE) -C elsa clean
	$(MAKE) -C ellcc clean

distclean:
	$(MAKE) -C smbase distclean
	$(MAKE) -C basic distclean
	$(MAKE) -C lex distclean
	$(MAKE) -C ast distclean
	$(MAKE) -C elkhound distclean
	$(MAKE) -C elsa distclean
	$(MAKE) -C ellcc distclean

doc:
	$(MAKE) -C smbase doc
	$(MAKE) -C basic doc
	$(MAKE) -C lex doc
	$(MAKE) -C ast doc
	$(MAKE) -C elkhound doc
	$(MAKE) -C elsa doc
	$(MAKE) -C ellcc doc
