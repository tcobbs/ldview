CSRCS = 
CCSRCS = \
	LDLibraryUpdateInfo.cpp \
	LDLibraryUpdater.cpp \
	LDModelParser.cpp \
	LDPreferences.cpp \
	LDrawModelViewer.cpp
EXECS= libLDraw.a
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -O3 -Wall -D_GNU_SOURCE -D_QT
INCLUDE = -I../ -I$(QTDIR)/include -I../include
LIBDIRS =
HOSTLIBS =

debug: CFLAGS = -o $(OBJDIR)/$*.o -g -Wall -D_GNU_SOURCE -D_QT
debug: all

-include ../Makefile.inc
-include .depend
