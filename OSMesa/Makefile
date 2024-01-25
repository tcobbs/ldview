POSTFIX = -osmesa
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

ifeq ($(shell if [ -f ../lib/lib3ds-64.a -a -f ../lib/lib3ds.a ] ; then echo OK; fi),OK)
EXPORT_3DS=-DEXPORT_3DS
endif

LDLIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib -L../../LDExporter
LIBDIRS = $(LDLIBDIRS) -L/usr/X11R6/lib
LIBS = -lLDraw$(POSTFIX) -lTRE$(POSTFIX) -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX) -lLDExporter$(POSTFIX) -lgl2ps -l$(LIB_PNG) -lz -lOSMesa -lGLU -lGL -ljpeg -ltinyxml -L ../../lib
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a ../TRE/libTRE$(POSTFIX).a ../LDLib/libLDraw$(POSTFIX).a ../LDExporter/libLDExporter$(POSTFIX).a
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO TESTING="$(TESTING)"

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE -O3 $(EXPORT_3DS)

ifeq ("$(USE_CPP11)","YES")

CFLAGS += -DUSE_CPP11 -std=c++11
LIBS   += -lpthread
endif

CSRCS = $(wildcard *.c)
CCSRCS =  ldview.cpp

BASEDIR=$(realpath $(shell if test -d 3rdParty ; then pwd ; else if test -d ../3rdParty ; then echo .. ;fi;fi))

ifeq ($(shell uname -s), Darwin)
CFLAGS += 
TESTING += -L../../lib/MacOSX
LIB_PNG=png16
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include

ifeq ("$(LOCAL_OSMESA)","YES")

LIBDIRS += -L$(BASEDIR)/3rdParty/osmesa/lib
INCLUDE += -I../3rdParty/osmesa/include
LIBS += -lglapi
endif

ifeq ($(shell uname -m), x86_64)
ifeq ($(LDVDEV32), YES)
CFLAGS += -m32
ARCH32 = -m32
LIBDIRS += -L/usr/lib32
ifeq ($(shell if [ -f ../lib/lib3ds.a ] ; then echo OK; fi),OK)
LIBS += -l3ds
endif
else
ifeq ($(shell if [ -f ../lib/lib3ds-64.a ] ; then echo OK; fi),OK)
LIBS += -l3ds-64
endif
endif
else
ifeq ($(shell if [ `uname -m` = i386 -o `uname -m` = i486 -o `uname -m` = i586 -o `uname -m` = i686 ]; then echo OK; fi),OK)
ifeq ($(shell if [ -f ../lib/lib3ds-64.a -a -f ../lib/lib3ds.a ] ; then echo OK; fi),OK)
LIBS += -l3ds
endif
endif
endif

ifeq ($(shell uname), FreeBSD)
INCLUDE += -I/usr/local/include
LIBDIRS += -L/usr/local/lib
endif

# slurm is media.peeron.com
ifeq ($(shell hostname), slurm)
INCLUDE += -I../../Mesa-7.0.2/include
LIBDIRS += -L../../../Mesa-7.0.2/lib
#STATIC = -static
endif

ifeq ($(shell hostname | cut -d. -f2-), pair.com)
# CFLAGS += -DNO_WSTRING -DNO_WCSTOUL -DTC_NO_UNICODE -DNO_WPRINTF
INCLUDE += -I../../Mesa-7.11/include -I/usr/local/include
LIBDIRS += -L../../../Mesa-7.11/lib -L/usr/local/lib/pth -L/usr/local/lib
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
	cd ../TCFoundation; $(MAKE) $(MAKEMODE)

../LDLoader/libLDLoader$(POSTFIX).a:
	cd ../LDLoader; $(MAKE) $(MAKEMODE)

../TRE/libTRE$(POSTFIX).a:
	cd ../TRE; $(MAKE) $(MAKEMODE)

../LDLib/libLDraw$(POSTFIX).a:
	cd ../LDLib; $(MAKE) $(MAKEMODE)

../LDExporter/libLDExporter$(POSTFIX).a:
	cd ../LDExporter; $(MAKE) $(MAKEMODE)

ifneq ($(shell if test -f /usr/include/tinyxml.h ; then echo OK ; fi),OK)
LIBS +=-L$(BASEDIR)/3rdParty/tinyxml
ldview:    $(BASEDIR)/3rdParty/tinyxml/libtinyxml.a
$(BASEDIR)/3rdParty/tinyxml/libtinyxml.a: $(BASEDIR)/3rdParty/tinyxml/*.cpp $(BASEDIR)/3rdParty/tinyxml/*.h
	cd $(BASEDIR)/3rdParty/tinyxml/ ; $(MAKE) -f Makefile.pbartfai
tinyxmlclean:
	cd ../3rdParty/tinyxml ; $(MAKE) -f Makefile.pbartfai clean
clean: tinyxmlclean
endif

ifneq ($(shell if test -f /usr/include/gl2ps.h ; then echo OK ; fi),OK)
LIBS +=-L$(BASEDIR)/3rdParty/gl2ps
ldview: $(BASEDIR)/3rdParty/gl2ps/libgl2ps.a
$(BASEDIR)/3rdParty/gl2ps/libgl2ps.a: $(BASEDIR)/3rdParty/gl2ps/*.c $(BASEDIR)/3rdParty/gl2ps/*.h
	cd $(BASEDIR)/3rdParty/gl2ps ; $(MAKE)

gl2psclean:
	cd ../3rdParty/gl2ps ; $(MAKE) clean
clean: gl2psclean
endif

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

Headerize: Headerize.o $(LDLIBS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) $(TESTING) -o ../Headerize Headerize.o $(LIBDIRS) -L../TCFoundation -lTCFoundation-osmesa -ljpeg -l$(LIB_PNG) -lz

LDViewMessages.h: Headerize ../LDViewMessages.ini
	./Headerize ../LDViewMessages.ini

StudLogo.h: Headerize ../Textures/StudLogo.png
	./Headerize ../Textures/StudLogo.png

ldview.o: StudLogo.h LDViewMessages.h

ldview: $(LDLIBS) $(OBJS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) $(TESTING) -o ../ldview $(OBJS) $(LIBDIRS) $(LIBS)

webgrabber:	$(OBJS)
			$(CC) -o webgrabber $(OBJS) $(LIBDIRS) $(LIBS)

clean: MAKEMODE = clean POSTFIX=-osmesa
clean:
	cd ../TRE ;          $(MAKE) $(MAKEMODE)
	cd ../LDLib ;        $(MAKE) $(MAKEMODE)
	cd ../TCFoundation ; $(MAKE) $(MAKEMODE)
	cd ../LDLoader ;     $(MAKE) $(MAKEMODE)
	cd ../LDExporter ;   $(MAKE) $(MAKEMODE)
	if [ -d $(OBJDIR) ] ; then	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) ldview core Headerize StudLogo.h

debug: CFLAGSLOC = -g -DUNZIP_CMD
debug: MAKEMODE = debug POSTFIX=-osmesa USE_BOOST=NO
debug: all

test: all
	@if [ ! -f ~/.ldviewrc  ] ; then                      \
		if [ ! -f ~/.config/LDView/ldviewrc ] ; then      \
			cp ldviewrc.sample ~/.ldviewrc;               \
			echo Default .ldviewrc created;               \
		fi                                                \
	fi
	@if [ -f ~/.ldviewrc ] ; then                                        \
		if [ -d "`grep ^LDrawDir ~/.ldviewrc | cut -f2 -d=`" ] ; then    \
			echo LDraw dir \(~/.ldviewrc\):                              \
				`grep ^LDrawDir ~/.ldviewrc | cut -f2 -d=` ;             \
		else                                                             \
			echo ERROR LDraw dir                                         \
				`grep ^LDrawDir ~/.ldviewrc | cut -f2 -d=`               \
				not found \(~/.ldviewrc\) ;                              \
			exit 1;                                                      \
		fi                                                               \
	elif [ -f ~/.config/LDView/ldviewrc ] ; then                         \
		if [ -d "`grep ^LDrawDir ~/.config/LDView/ldviewrc | cut -f2 -d=`" ] ; then   \
			echo LDraw dir \(~/.config/LDView/ldviewrc\):                \
			`grep ^LDrawDir ~/.config/LDView/ldviewrc | cut -f2 -d=`;    \
		else                                                             \
			echo ERROR LDraw dir                                         \
				`grep ^LDrawDir ~/.config/LDView/ldviewrc | cut -f2 -d=` \
				not found \(~/.config/LDView/ldviewrc\);                 \
			exit 1;                                                      \
		fi                                                               \
	elif [ -d /usr/share/ldraw ] ; then                                  \
		echo LDraw dir default: /usr/share/ldraw ;                       \
	else                                                                 \
		echo LDraw dir not found ;                                       \
		exit 1;                                                          \
	fi
	@if [ -f /tmp/8464.png ] ; then rm -f /tmp/8464.png ; fi
	./ldview ../8464.mpd -SaveSnapshot=/tmp/8464.png \
	-SaveWidth=1024 -SaveHeight=1024 -ShowErrors=0 -SaveActualSize=0
	@if [ -f /tmp/8464.png ] ; then                     \
		file /tmp/8464.png ;                            \
		for a in eog gthumb ristretto ; do 						\
			if which $$a >/dev/null 2>/dev/null ; then  \
				$$a /tmp/8464.png; 						\
				break ;									\
			fi											\
		done                                            \
	fi

install: ldview
	install -D -m 755 ldview $(PREFIX)/usr/bin/ldview
	install -D -m 644 ldview.1 $(PREFIX)/usr/share/man/man1/ldview.1
	gzip -9f $(PREFIX)/usr/share/man/man1/ldview.1
	install -D -m 644 ldviewrc.sample $(PREFIX)/usr/share/ldview/ldviewrc.sample
