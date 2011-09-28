EXECS= libLDExporter$(POSTFIX).a
CFLAGSLOC = -O3 -I ../3rdParty/lib3ds -I ../3rdParty/tinyxml -D TIXML_USE_STL
LIBDIRS =
HOSTLIBS =

-include ../Makefile.inc

debug: CFLAGSLOC = -g -I ../3rdParty/lib3ds -I ../3rdParty/tinyxml -D TIXML_USE_STL
debug: all

-include .depend
