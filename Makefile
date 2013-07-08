# Makefile for Linux/Unix based systems
# Thanks to Johannes Schauer
#
INSTALL_ROOT=$(DESTDIR)
export INSTALL_ROOT

all:
	cd QT; \
	if which qmake >/dev/null 2>/dev/null ; \
	then qmake ; lrelease LDView.pro ; \
	else qmake-qt4 ; lrelease-qt4 LDView.pro ; fi
	cd QT; $(MAKE)

install:
	cd QT; $(MAKE) install

clean:
ifneq ($(wildcard QT/Makefile),)
	cd QT; $(MAKE) distclean
endif

