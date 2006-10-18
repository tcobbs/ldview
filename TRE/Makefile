CSRCS = 
CCSRCS = \
	TRECamera.cpp \
	TREColoredShapeGroup.cpp \
	TREFacing.cpp \
	TREMainModel.cpp \
	TREModel.cpp \
	TREShapeGroup.cpp \
	TRESmoother.cpp \
	TRESubModel.cpp \
	TRETransShapeGroup.cpp \
	TREVertexArray.cpp \
	TREVertexStore.cpp
EXECS= libTRE.a
ifeq ($(VENDOR),apple)
EXTRACFLAGS = -arch i386 -arch ppc
endif
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -O3 -Wall -D_GNU_SOURCE -D_QT $(EXTRACFLAGS)
INCLUDE = -I../ -I../include
LIBDIRS =
HOSTLIBS =

debug: CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -g -Wall -D_GNU_SOURCE -D_QT $(EXTRACFLAGS)
debug: all

-include ../Makefile.inc
-include .depend
