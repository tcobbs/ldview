POSTFIX = -osmesa
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM
QTDIR =
LDLIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib
LIBDIRS = $(LDLIBDIRS) -L../../boost/lib -L/usr/X11R6/lib
LIBS = -lLDraw$(POSTFIX) -lTRE$(POSTFIX) -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -lpng -lz -lOSMesa -lGLU -lboost_thread -lpthread -ljpeg
STATIC =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a ../TRE/libTRE$(POSTFIX).a ../LDLib/libLDraw$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3

CSRCS = $(wildcard *.c)
CCSRCS =  ldview.cpp

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


$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

Headerize: Headerize.o $(LDLIBS)
	cd $(OBJDIR); $(CC) $(STATIC) -o ../Headerize Headerize.o $(LIBDIRS) $(LIBS)

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
	cd $(OBJDIR); $(CC) $(STATIC) -o ../ldview $(OBJS) $(LIBDIRS) $(LIBS)

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
