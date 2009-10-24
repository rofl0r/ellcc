# Makefile for ELLCC distribution

# just do the obvious recursive thing
all:
	$(MAKE) -C SMBase
	$(MAKE) -C Basic
	$(MAKE) -C Lex
	$(MAKE) -C Ast
	$(MAKE) -C Elkhound
	$(MAKE) -C Elsa
	$(MAKE) -C Main

check:
	$(MAKE) -C smbase check
	$(MAKE) -C Ast check
	$(MAKE) -C Elkhound check
	$(MAKE) -C Elsa check
	$(MAKE) -C Main check

clean:
	$(MAKE) -C smbase clean
	$(MAKE) -C Basic clean
	$(MAKE) -C Lex clean
	$(MAKE) -C Ast clean
	$(MAKE) -C Elkhound clean
	$(MAKE) -C Elsa clean
	$(MAKE) -C Main clean

distclean:
	$(MAKE) -C smbase distclean
	$(MAKE) -C Basic distclean
	$(MAKE) -C Lex distclean
	$(MAKE) -C Ast distclean
	$(MAKE) -C Elkhound distclean
	$(MAKE) -C Elsa distclean
	$(MAKE) -C Main distclean

doc:
	$(MAKE) -C smbase doc
	$(MAKE) -C Basic doc
	$(MAKE) -C Lex doc
	$(MAKE) -C Ast doc
	$(MAKE) -C Elkhound doc
	$(MAKE) -C Elsa doc
	$(MAKE) -C Main doc
