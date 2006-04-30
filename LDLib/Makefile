CC = g++
RM = rm -f
RMDIR = rm -rf
AR = ar rcs
MAKEDEPEND = gcc -MM

OBJDIR = .obj

VPATH = $(OBJDIR)

CSRCS = 
CCSRCS = \
	LDLibraryUpdateInfo.cpp \
	LDLibraryUpdater.cpp \
	LDModelParser.cpp \
	LDrawModelViewer.cpp
EXECS= libLDraw.a
SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -O3 -Wall -D_GNU_SOURCE -D_QT
INCLUDE = -I../ -I$(QTDIR)/include -I../include -I../boost/include
LIBDIRS =
#HOSTLIBS = -lxnet
HOSTLIBS =
LIBS = -lm -lTCFoundation $(HOSTLIBS)

.SUFFIXES:

.SUFFIXES:	.cpp .o .c

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

all:	$(OBJDIR) libLDraw.a

debug: CFLAGS = -o $(OBJDIR)/$*.o -g -Wall -D_GNU_SOURCE -D_QT
debug: all

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then	\
		mkdir $(OBJDIR);				\
	fi

libLDraw.a:	$(OBJS)
	rm -f libLDraw.a
	cd $(OBJDIR); $(AR) ../libLDraw.a $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

depend:
	rm -f .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) libLDraw.a core

-include .depend
