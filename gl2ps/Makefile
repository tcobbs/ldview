EXECS= libgl2ps$(POSTFIX).a
CFLAGSLOC = -O3  -D GL2PS_HAVE_ZLIB -D GL2PS_HAVE_LIBPNG
LIBDIRS =
HOSTLIBS =

-include ../Makefile.inc

debug: CFLAGSLOC = -g
debug: all

-include .depend
