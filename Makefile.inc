ifeq ($(shell uname), FreeBSD)
CC = cc
CXX = c++
else
CC = gcc
CXX = g++
ifeq ($(shell if [ `uname -m` = x86_64 -o `uname -m` = i386 -o `uname -m` = i486 -o `uname -m` = i586 -o `uname -m` = i686 ]; then echo OK; fi),OK)
ifeq ($(shell if [ -f ../lib/lib3ds-64.a -a -f ../lib/lib3ds.a ] ; then echo OK; fi),OK)
EXPORT_3DS=-DEXPORT_3DS
endif
endif
endif
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE $(EXPORT_3DS) -DHAVE_MINIZIP

CSRCS = $(wildcard *.c)
CCSRCS =  $(wildcard *.cpp)

ifeq ($(shell uname -s), Darwin)
CFLAGS += -DUSE_CPP11 -std=c++11 -I../3rdParty -I../3rdParty/libpng -I../3rdParty/libpng/MacOSX -I.. -I../libpng/MacOSX -I../libpng -I../3rdParty/libjpeg -I../3rdParty/libjpeg/MacOSX -DGL_SILENCE_DEPRECATION
AR = libtool -static  -o
CC = clang
CXX = clang++
else
AR = ar rcs
endif

INCLUDE = -I.. #-I../3rdParty

ifeq ($(shell uname -m), x86_64)
ifeq ($(LDVDEV64), NO)
CFLAGS += -m32
endif
endif

ifeq ($(shell uname), FreeBSD)
INCLUDE += -I/usr/local/include
endif

# slurm is media.peeron.com
ifeq ($(shell hostname), slurm)
INCLUDE += -I../../Mesa-7.0.2/include
endif

ifeq ($(shell hostname | cut -d. -f2-), pair.com)
# CFLAGS += -DNO_WSTRING -DTC_NO_UNICODE
INCLUDE += -I../../Mesa-7.11/include -I/usr/local/include
CFLAGS += -D_GL_POPCOLOR_BROKEN
//INCLUDE += -nostdinc -Iinc -I../../Mesa-7.11/include -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include/g++-v3 -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include -I/usr/local/lib/gcc-lib/i386-portbld-freebsd4.8/3.2.3/include/g++-v3/i386-portbld-freebsd4.8
endif

ifeq ("$(USE_BOOST)","NO")

CFLAGS += -D_NO_BOOST

endif

ifeq ("$(USE_CPP11)","YES")

CFLAGS += -DUSE_CPP11 -std=c++11

endif

ifeq ("$(USE_CPP17)","YES")
CFLAGS += -DUSE_CPP11 -std=c++17
endif

ifeq ("$(POSTFIX)","-osmesa")

CFLAGS += -D_OSMESA

else

CFLAGS += -D_QT

#ifeq ($(shell if [ -d /usr/include/qt4 ]; then echo OK; fi),OK)
#INCLUDE += -I/usr/include/qt4
#endif

#ifeq ($(shell if [ -d /usr/lib/qt4/include ]; then echo OK; fi),OK)
#INCLUDE += -I/usr/lib/qt4/include
#endif
endif

ifeq ($(shell if [ -f /usr/include/minizip/mz_compat.h ] ; then echo OK; fi),OK)
CFLAGS += -DHAVE_MINIZIP_NG
endif

BASEDIR=$(realpath $(shell if test -d 3rdParty ; then pwd ; else if test -d ../3rdParty ; then echo .. ;fi;fi))

ifeq ($(shell if test -f /usr/include/tinyxml.h ; then echo OK ; fi),OK)
TINYXML=
TINYXMLLIB=
else
TINYXML=-I$(BASEDIR)/3rdParty/tinyxml
TINYXMLLIB=-L$(BASEDIR)/3rdParty/tinyxml
endif

ifeq ($(shell if test -f /usr/include/gl2ps.h ; then echo OK ; fi),OK)
GL2PS=
GL2PSLIB=
else
GL2PS=-I$(BASEDIR)/3rdParty/gl2ps
GL2PSLIB=-L$(BASEDIR)/3rdParty/gl2ps
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
	$(CXX) $(CFLAGS) $(INCLUDE) $(CFLAGSLOC) -c $<

.c.o:
	$(CC) $(shell echo $(CFLAGS) | sed "s/-std=c++17//g" | sed "s/-std=c++11//g" )  $(INCLUDE) $(CFLAGSLOC) -c $<

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
	if [ -d $(OBJDIR) ]; then 	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) $(EXECS) core

systest:
	@echo GL2PS=${GL2PS}
	@echo TINYXML=${TINYXML}
