CC = g++
RM = rm -f
RMDIR = rmdir --ignore-fail-on-non-empty
AR = ar rcs
MAKEDEPEND = gcc -MM

OBJDIR = .obj

VPATH = $(OBJDIR)

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
	TCMutex.cpp \
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
	TCThread.cpp \
	TCThreadCondition.cpp \
	TCThreadManager.cpp \
	TCUnzip.cpp \
	TCUserDefaults.cpp \
	TCVector.cpp \
	TCWebClient.cpp \
	mystring.cpp
EXECS= libTCFoundation.a
SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o -O3 $(TESTING) -Wall -D_GNU_SOURCE -D_QT
INCLUDE = -I../ -I$(QTDIR)/include
LIBDIRS = -L$(QTDIR)/lib
#HOSTLIBS = -lxnet
HOSTLIBS =
LIBS = -lm -lpthread -lqt-mt $(HOSTLIBS)

.SUFFIXES:

.SUFFIXES:	.cpp .o .c

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

all:	$(OBJDIR) libTCFoundation.a

debug: CFLAGS = -o $(OBJDIR)/$*.o -g -Wall -D_GNU_SOURCE -D_QT
debug: all

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then	\
		mkdir $(OBJDIR);				\
	fi

libTCFoundation.a:	$(OBJS)
	rm -f libTCFoundation.a
	cd $(OBJDIR); $(AR) ../libTCFoundation.a $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

depend:
	rm -f .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) libTCFoundation.a core

-include .depend
# DO NOT DELETE
