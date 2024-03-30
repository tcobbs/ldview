EXECS= libminizip$(POSTFIX).a
CFLAGSLOC = -O3
LIBDIRS =
HOSTLIBS =

.PHONY: iowin32.o miniunz.o minizip.o
-include ../../Makefile.inc
SRCS = ioapi.c mztools.c unzip.c zip.c
OBJS = ioapi.o mztools.o unzip.o zip.o

debug: CFLAGSLOC = -g
debug: all

-include .depend
