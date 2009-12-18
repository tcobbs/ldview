EXECS= libgl2ps$(POSTFIX).a
CFLAGSLOC = -O3  -DGL2PS_HAVE_ZLIB -DGL2PS_HAVE_LIBPNG
LIBDIRS =
HOSTLIBS =

-include ../Makefile.inc

debug: CFLAGSLOC = -g
debug: all

-include .depend
