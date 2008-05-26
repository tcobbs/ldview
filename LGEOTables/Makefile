APP = LGEOTables
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

LIBDIRS =
LIBS = -ltinyxml
STATIC =
ARCH32 =

CFLAGS = -o $(OBJDIR)/$*.o -Wall -D_GNU_SOURCE -O3 -D_NO_BOOST

CSRCS = $(wildcard *.c)
CCSRCS = $(wildcard *.cpp)

ifeq ($(shell uname -s), Darwin)
CFLAGS += -arch i386 -arch ppc
endif

INCLUDE = -I.. -I../include
OBJDIR = .obj$(POSTFIX)
VPATH = $(OBJDIR)

SRCS = $(CSRCS) $(CCSRCS)
COBJS = $(CSRCS:.c=.o)
CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(COBJS) $(CCOBJS)

.SUFFIXES:

.SUFFIXES:  .cpp .o .c

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

all:    $(OBJDIR) $(APP)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

$(APP):   $(OBJS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) -o ../$(APP) $(OBJS) $(LIBDIRS) $(LIBS)

clean:
	if [ -d $(OBJDIR) ] ; then	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) $(APP)
