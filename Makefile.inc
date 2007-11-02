CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE

CSRCS = $(wildcard *.c)
CCSRCS =  $(wildcard *.cpp)

ifeq ($(shell uname -s), Darwin)
CFLAGS += -arch i386 -arch ppc
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include -I../boost/include

ifeq ($(shell hostname | cut -d. -f2-), pair.com)
CFLAGS += -DNO_WSTRING -DTC_NO_UNICODE
INCLUDE += -I../../Mesa-7.0.1/include
//INCLUDE += -nostdinc -Iinc -I../../Mesa-7.0.1/include -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include/g++-v3 -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include/g++-v3/i386-portbld-freebsd4.8
endif

ifeq ("$(QTDIR)","")

CFLAGS += -D_OSMESA

else

CFLAGS += -D_QT

ifeq ($(shell if [ -d $(QTDIR)/include ]; then echo OK; fi),OK)
INCLUDE += -I$(QTDIR)/include
endif

ifeq ($(shell if [ -d $(QTDIR)/include/Qt3Support ]; then echo OK; fi),OK)
INCLUDE += -I$(QTDIR)/include/Qt
CFLAGS += -DQT3_SUPPORT
endif

ifeq ($(shell if [ -d /usr/include/qt3 ]; then echo OK; fi),OK)
INCLUDE += -I/usr/include/qt3
endif

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
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

all:    $(OBJDIR) $(EXECS)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

$(EXECS):   $(OBJS)
	$(RM) $(EXECS)
	cd $(OBJDIR); $(AR) ../$(EXECS) $(OBJS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) $(EXECS) core

