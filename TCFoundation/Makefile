CSRCS = 
CCSRCS = \
	TCArray.cpp \
	TCAlert.cpp \
	TCAlertManager.cpp \
	TCAutoreleasePool.cpp \
	TCBinaryObjectTree.cpp \
	TCBinaryTree.cpp \
	TCBmpImageFormat.cpp \
	TCDictionary.cpp \
	TCImage.cpp \
	TCImageFormat.cpp \
	TCLocalStrings.cpp \
	TCNetwork.cpp \
	TCNetworkClient.cpp \
	TCObject.cpp \
	TCObjectArray.cpp \
	TCObjectTree.cpp \
	TCPngImageFormat.cpp \
	TCProgressAlert.cpp \
	TCSortedStringArray.cpp \
	TCStringArray.cpp \
	TCStringObjectTree.cpp \
	TCUnzip.cpp \
	TCUserDefaults.cpp \
	TCVector.cpp \
	TCWebClient.cpp \
	mystring.cpp
EXECS= libTCFoundation.a
CDEFINES = -D_GNU_SOURCE -D_QT -DUNZIP_CMD
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall $(CDEFINES)
CFLAGS = -o $(OBJDIR)/$*.o -O3 $(TESTING) -Wall $(CDEFINES) $(EXTRACFLAGS)
INCLUDE = -I../ -I$(QTDIR)/include -I../include
LIBDIRS = -L$(QTDIR)/lib
HOSTLIBS =

debug: CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -g -Wall $(CDEFINES) $(EXTRACFLAGS)
debug: all

-include ../Makefile.inc
-include .depend
# DO NOT DELETE
