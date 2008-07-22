## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = cygnus
SUBDIRS = plugins

Makefile: Makefile.in config.status
	CONFIG_FILES=Makefile CONFIG_HEADERS= $(SHELL) ./config.status

config.status: configure
	$(SHELL) config.status --recheck

.PHONY: info dvi ps pdf html
.PHONY: install-info install-dvi install-ps install-pdf install-html

info dvi ps pdf html:
install-info install-dvi install-ps install-pdf install-html:

