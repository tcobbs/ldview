CC = g++
RM = rm -f
RMDIR = rm -rf
AR = ar rcs
MAKEDEPEND = gcc -MM

OBJDIR = .obj

VPATH = $(OBJDIR)

CSRCS = \
	LDrawIni.c
CCSRCS = \
	LDLActionLine.cpp \
	LDLCommentLine.cpp \
	LDLConditionalLineLine.cpp \
	LDLEmptyLine.cpp \
	LDLError.cpp \
	LDLFileLine.cpp \
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
SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)
#CFLAGS = -g $(TESTING) -pg -Wall
#CFLAGS = -o $(OBJDIR)/$*.o -g $(TESTING) -Wall -D_GNU_SOURCE -D_QT
CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -O3 -Wall -D_GNU_SOURCE -D_QT
INCLUDE = -I../
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

all:	$(OBJDIR) libLDLoader.a

debug: CFLAGS = -o $(OBJDIR)/$*.o -g -Wall -D_GNU_SOURCE -D_QT
debug: all

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then	\
		mkdir $(OBJDIR);				\
	fi

libLDLoader.a:	$(OBJS)
	rm -f libLDLoader.a
	cd $(OBJDIR); $(AR) ../libLDLoader.a $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

depend:
	rm -f .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) libLDLoader.a core

-include .depend
