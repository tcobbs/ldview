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

LDLIBDIRS = -L../../TCFoundation -L../../LDLoader
LIBDIRS = $(LDLIBDIRS)
LIBS = -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -l$(LIB_PNG) -lz -ljpeg -lminizip
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO TESTING="$(TESTING)"

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3 -DHAVE_MINIZIP

ifeq ("$(USE_CPP11)","YES")

CFLAGS += -DUSE_CPP11 -std=c++11
LIBS   += -lpthread
endif

CCSRCS = $(wildcard *.cpp)

ifeq ($(shell uname -s), Darwin)
CFLAGS += 
TESTING += -L../../lib/MacOSX
LIB_PNG=png16
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include

ifeq ($(shell uname), FreeBSD)
INCLUDE += -I/usr/local/include
LIBDIRS += -L/usr/local/lib
endif

OBJDIR = .obj$(POSTFIX)

VPATH = $(OBJDIR)

CCOBJS = $(CCSRCS:.cpp=.o)
OBJS = $(CCOBJS)

.SUFFIXES:

.SUFFIXES:  .cpp .o

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

debug: CFLAGSLOC = -g
debug: MAKEMODE += debug
debug: all

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

