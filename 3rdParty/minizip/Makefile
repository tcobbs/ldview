EXECS= libminizip.a
CFLAGSLOC = -O3
LIBDIRS =
HOSTLIBS =

.PHONY: iowin32.o miniunz.o minizip.o
-include ../../Makefile.inc
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING)
SRCS = ioapi.c mztools.c unzip.c zip.c
OBJS = ioapi.o mztools.o unzip.o zip.o

debug: CFLAGSLOC = -g
debug: all

-include .depend
