CC = g++
RM = rm -f
RMDIR = rmdir --ignore-fail-on-non-empty
AR = ar rcs
MAKEDEPEND = gcc -MM

OBJDIR = .obj

VPATH = $(OBJDIR)

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

all:	$(OBJDIR) libTRE.a

debug: CFLAGS = -o $(OBJDIR)/$*.o -g -Wall -D_GNU_SOURCE -D_QT
debug: all

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then	\
		mkdir $(OBJDIR);				\
	fi

libTRE.a:	$(OBJS)
	rm -f libTRE.a
	cd $(OBJDIR); $(AR) ../libTRE.a $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

depend:
	rm -f .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) libTRE.a core

-include .depend
