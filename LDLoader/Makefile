CSRCS = \
	LDrawIni.c
CCSRCS = \
	LDLActionLine.cpp \
	LDLCommentLine.cpp \
	LDLConditionalLineLine.cpp \
	LDLEmptyLine.cpp \
	LDLError.cpp \
	LDLFindFileAlert.cpp \
	LDLFileLine.cpp \
	LDLFindFileAlert.cpp \
	LDLLineLine.cpp \
	LDLMainModel.cpp \
	LDLModel.cpp \
	LDLModelLine.cpp \
	LDLPalette.cpp \
	LDLQuadLine.cpp \
	LDLShapeLine.cpp \
	LDLTriangleLine.cpp \
	LDLUnknownLine.cpp
EXECS= libLDLoader.a
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -O3 -Wall -D_GNU_SOURCE -D_QT $(EXTRACFLAGS)
INCLUDE = -I../
LIBDIRS =
HOSTLIBS =

debug: CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -g -Wall -D_GNU_SOURCE -D_QT $(EXTRACFLAGS)
debug: all

-include ../Makefile.inc
-include .depend
