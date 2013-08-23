EXECS= libLDExporter$(POSTFIX).a
CFLAGSLOC = -O3 -I ../3rdParty/lib3ds -D TIXML_USE_STL ${TINYXML}
LIBDIRS =
HOSTLIBS =

-include ../Makefile.inc

debug: CFLAGSLOC = -g -I ../3rdParty/lib3ds -D TIXML_USE_STL ${TINYXML}
debug: all

-include .depend
