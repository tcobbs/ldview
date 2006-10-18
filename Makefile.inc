CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

ifeq ($(shell uname -s), Darwin)
EXTRACFLAGS = -arch i386 -arch ppc
AR=libtool -static  -o
else
AR = ar rcs
endif

OBJDIR = .obj

VPATH = $(OBJDIR)

SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)

.SUFFIXES:

.SUFFIXES:  .cpp .o .c

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<

all:    $(OBJDIR) $(EXECS)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	rm -f .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

$(EXECS):   $(OBJS)
	rm -f $(EXECS)
	cd $(OBJDIR); $(AR) ../$(EXECS) $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) $(EXECS) core

