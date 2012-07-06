CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM

EXPORT_3DS=-DEXPORT_3DS

CFLAGS = -o $(OBJDIR)/$*.o $(TESTING) -Wall -D_GNU_SOURCE $(EXPORT_3DS)

CSRCS = $(wildcard *.c)
CCSRCS =  $(wildcard *.cpp)

ifeq ($(shell uname -s), Darwin)
CFLAGS += -arch i386 -arch ppc
AR = libtool -static  -o
else
AR = ar rcs
endif

INCLUDE = -I.. -I../include -I../boost/include

ifeq ($(shell uname -m), x86_64)
ifeq ($(LDVDEV64), NO)
CFLAGS += -m32
endif
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

ifeq ("$(POSTFIX)","-osmesa")

CFLAGS += -D_OSMESA

else

CFLAGS += -D_QT

#ifeq ($(shell if [ -d $(QTDIR)/include ]; then echo OK; fi),OK)
#INCLUDE += -I$(QTDIR)/include
#endif

#ifeq ($(shell if [ -d $(QTDIR)/include/Qt3Support ]; then echo OK; fi),OK)
#INCLUDE += -I$(QTDIR)/include/Qt
#CFLAGS += -DQT3_SUPPORT
#endif

ifeq ($(shell if [ -d /usr/include/qt4 ]; then echo OK; fi),OK)
INCLUDE += -I/usr/include/qt4
endif

ifeq ($(shell if [ -d /usr/lib/qt4/include ]; then echo OK; fi),OK)
INCLUDE += -I/usr/lib/qt4/include
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

