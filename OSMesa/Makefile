EXECS = ldview
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM
QTDIR =
LIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib -L../../boost/lib -L/usr/X11R6/lib
LIBS = -lLDraw -lTRE -lLDLoader -lTCFoundation -lpng -lz -lOSMesa -lGLU -lboost_thread -lpthread
STATIC =
LDLIBS = ../TCFoundation/libTCFoundation.a ../LDLoader/libLDLoader.a ../TRE/libTRE.a ../LDLib/libLDraw.a
MAKEMODE =

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
CFLAGS += -DNO_WSTRING -DNO_WCSTOUL -DTC_NO_UNICODE -DNO_WPRINTF
INCLUDE += -I../../Mesa-7.0.1/include
LIBDIRS += -L../../../Mesa-7.0.1/lib -L/usr/local/lib/pth -L/usr/local/lib
LIBS += -lpth
STATIC = -static
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

../TCFoundation/libTCFoundation.a:
	cd ../TCFoundation; make $(MAKEMODE)

../LDLoader/libLDLoader.a:
	cd ../LDLoader; make $(MAKEMODE)

../TRE/libTRE.a:
	cd ../TRE; make $(MAKEMODE)

../LDLib/libLDraw.a:
	cd ../LDLib; make $(MAKEMODE)


$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

$(EXECS):   $(OBJS) $(LDLIBS)
	$(RM) $(EXECS)
	cd $(OBJDIR); $(CC) $(STATIC) -o ../$(EXECS) $(OBJS) $(LIBDIRS) $(LIBS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean:
	cd $(OBJDIR); $(RM) $(OBJS)
	$(RMDIR) $(OBJDIR)
	$(RM) $(EXECS) core

debug: CFLAGSLOC = -g -DUNZIP_CMD
debug: MAKEMODE = debug
debug: all
