POSTFIX=-osmesa
ifeq ($(shell uname), FreeBSD)
CC = c++
else
CC = g++
endif
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

ifeq ("$(LIB_PNG)","")

LIB_PNG=png

endif

ifeq ($(shell if $(CC) -v --help 2>&1| grep -q c++11 ; then echo OK; fi), OK)
	TESTING+=-DUSE_CPP11
endif

LDLIBDIRS = -L../../TCFoundation -L../../LDLoader -L../../LDLib
LIBDIRS = $(LDLIBDIRS) -L/usr/X11R6/lib
LIBS = -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -l$(LIB_PNG) -lz -ljpeg -lminizip
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO TESTING="$(TESTING)"

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3 $(EXPORT_3DS) -DHAVE_MINIZIP

ifeq ("$(USE_CPP11)","YES")

CFLAGS += -DUSE_CPP11 -std=c++11
LIBS   += -lpthread
endif

CSRCS = $(wildcard *.c)
CCSRCS = AlertHandler.cpp  Consts.cpp  main.cpp  PartsCatalog.cpp

BASEDIR=$(realpath $(shell if test -d 3rdParty ; then pwd ; else if test -d ../3rdParty ; then echo .. ;fi;fi))

ifeq ($(shell uname -s), Darwin)
CFLAGS += 
TESTING += -L../../lib/MacOSX
LIB_PNG=png16
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include #-I../3rdParty

ifeq ($(shell uname), FreeBSD)
INCLUDE += -I/usr/local/include
LIBDIRS += -L/usr/local/lib
endif

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

all:    $(OBJDIR) BuildPartsCatalog

../TCFoundation/libTCFoundation$(POSTFIX).a:
	cd ../TCFoundation; $(MAKE) $(MAKEMODE)

../LDLoader/libLDLoader$(POSTFIX).a:
	cd ../LDLoader; $(MAKE) $(MAKEMODE)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend


BuildPartsCatalog: $(LDLIBS) $(OBJS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) $(TESTING) -o ../BuildPartsCatalog $(OBJS) $(LIBDIRS) $(LIBS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean: MAKEMODE = clean POSTFIX=-osmesa
clean:
	cd ../TCFoundation ; $(MAKE) $(MAKEMODE)
	cd ../LDLoader ;     $(MAKE) $(MAKEMODE)
	$(RMDIR) $(OBJDIR)
	$(RM) BuildPartsCatalog

