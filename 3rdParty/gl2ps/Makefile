EXECS= libgl2ps$(POSTFIX).a
CFLAGSLOC = -O3  -DGL2PS_HAVE_ZLIB -DGL2PS_HAVE_LIBPNG
LIBDIRS =
HOSTLIBS =

.PHONY: gl2psTest.o  gl2psTestSimple.o
-include ../../Makefile.inc
SRCS = gl2ps.c 
OBJS = gl2ps.o

debug: CFLAGSLOC = -g
debug: all

-include .depend
