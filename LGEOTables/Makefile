APP = LGEOTables
CC = g++
RM = rm -f
RMDIR = rm -rf
MAKEDEPEND = gcc -MM
POSTFIX = -osmesa

LIBDIRS = -L../../TCFoundation -L../../TRE -L../../LDLoader -L../../LDLib -L../../LDExporter
LIBS = -ljpeg -lpng -lz -ltinyxml -lminizip -lLDraw$(POSTFIX) -lTRE$(POSTFIX) -lLDLoader$(POSTFIX) -lTCFoundation$(POSTFIX)
STATIC =
ARCH32 =
LDLIBS = ../TCFoundation/libTCFoundation$(POSTFIX).a ../LDLoader/libLDLoader$(POSTFIX).a ../TRE/libTRE$(POSTFIX).a ../LDLib/libLDraw$(POSTFIX).a

CFLAGS = -o $(OBJDIR)/$*.o -Wall -D_GNU_SOURCE -O3 -D_NO_BOOST -DHAVE_MINIZIP -D_OSMESA
MAKEMODE = POSTFIX=-osmesa USE_BOOST=NO TESTING="$(TESTING)"


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

all:	$(OBJDIR) $(APP)

../TCFoundation/libTCFoundation$(POSTFIX).a:
	cd ../TCFoundation; $(MAKE) $(MAKEMODE)

../LDLoader/libLDLoader$(POSTFIX).a:
	cd ../LDLoader; $(MAKE) $(MAKEMODE)

../TRE/libTRE$(POSTFIX).a:
	cd ../TRE; $(MAKE) $(MAKEMODE)

../LDLib/libLDraw$(POSTFIX).a:
	cd ../LDLib; $(MAKE) $(MAKEMODE)

$(OBJDIR):
	if [ ! -d $(OBJDIR) ]; then \
		mkdir $(OBJDIR);                \
	fi

depend:
	$(RM) .depend
	$(MAKEDEPEND) $(INCLUDE) $(SRCS) > .depend

$(APP):   $(OBJS) $(LDLIBS)
	cd $(OBJDIR); $(CC) $(STATIC) $(ARCH32) -o ../$(APP) $(OBJS) $(LIBDIRS) $(LIBS)

clean:	MAKEMODE = clean POSTFIX=-osmesa
clean:
	cd ../TRE ;          $(MAKE) $(MAKEMODE)
	cd ../LDLib ;        $(MAKE) $(MAKEMODE)
	cd ../TCFoundation ; $(MAKE) $(MAKEMODE)
	cd ../LDLoader ;     $(MAKE) $(MAKEMODE)
	if [ -d $(OBJDIR) ] ; then	\
		cd $(OBJDIR);			\
		$(RM) $(OBJS);			\
	fi
	$(RMDIR) $(OBJDIR)
	$(RM) $(APP)
