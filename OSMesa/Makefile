POSTFIX = -osmesa
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM
QTDIR =

LDLIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib -L../../gl2ps
LIBDIRS = $(LDLIBDIRS) -L/usr/X11R6/lib
LIBS = -lLDraw$(POSTFIX) -lTRE$(POSTFIX) -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -lgl2ps$(POSTFIX) -lpng -lz -lOSMesa -lGLU -ljpeg
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a ../TRE/libTRE$(POSTFIX).a ../LDLib/libLDraw$(POSTFIX).a ../gl2ps/libgl2ps$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3

CSRCS = $(wildcard *.c)
CCSRCS =  ldview.cpp

ifeq ($(shell uname -s), Darwin)
CFLAGS += -arch i386 -arch ppc
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include

ifeq ($(shell uname -m), x86_64)
CFLAGS += -m32
ARCH32 = -m32
LIBDIRS += -L/usr/lib32
endif

# slurm is media.peeron.com
ifeq ($(shell hostname), slurm)
INCLUDE += -I../../Mesa-7.0.2/include
LIBDIRS += -L../../../Mesa-7.0.2/lib
#STATIC = -static
endif

ifeq ($(shell hostname | cut -d. -f2-), pair.com)
CFLAGS += -DNO_WSTRING -DNO_WCSTOUL -DTC_NO_UNICODE -DNO_WPRINTF
INCLUDE += -I../../Mesa-7.0.1/include
LIBDIRS += -L../../../Mesa-7.0.1/lib -L/usr/local/lib/pth -L/usr/local/lib
LIBS += -lpth
STATIC = -static
endif

CFLAGS += -D_OSMESA -D_NO_BOOST

ifeq ($(shell if [ -d /usr/include/qt3 ]; then echo OK; fi),OK)
INCLUDE += -I/usr/include/qt3
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

all:    $(OBJDIR) ldview

../TCFoundation/libTCFoundation$(POSTFIX).a:
	cd ../TCFoundation; make $(MAKEMODE)

../LDLoader/libLDLoader$(POSTFIX).a:
	cd ../LDLoader; make $(MAKEMODE)

../TRE/libTRE$(POSTFIX).a:
	cd ../TRE; make $(MAKEMODE)

../LDLib/libLDraw$(POSTFIX).a:
	cd ../LDLib; make $(MAKEMODE)

../gl2ps/libgl2ps$(POSTFIX).a:
	cd ../gl2ps; make $(MAKEMODE)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

Headerize: Headerize.o $(LDLIBS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) -o ../Headerize Headerize.o $(LIBDIRS) $(LIBS)

LDViewMessages.h: Headerize ../LDViewMessages.ini
	./Headerize ../LDViewMessages.ini

StudLogo.h: Headerize ../Textures/StudLogo.png
	./Headerize ../Textures/StudLogo.png

BuildLibs:
	for i in $(LDLIBDIRS); do				\
		cd `echo $$i | sed "s/-L...//"`;	\
		make $(MAKEMODE);					\
	done

ldview:   BuildLibs StudLogo.h LDViewMessages.h $(LDLIBS) $(OBJS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) -o ../ldview $(OBJS) $(LIBDIRS) $(LIBS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean: MAKEMODE = clean POSTFIX=-osmesa
clean: BuildLibs
	if [ -d $(OBJDIR) ] ; then	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) ldview core Headerize StudLogo.h

debug: CFLAGSLOC = -g -DUNZIP_CMD
debug: MAKEMODE = debug POSTFIX=-osmesa
debug: all
