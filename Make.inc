#
#
#	Common rules for building LDView on Windows systems
#
#
#DEBUG=yes
#ANSI=yes
#NO_BOOST=yes

RFLAGS=/l 0x409 
CPPGLOBAL=/nologo /W4 /EHsc /Ox /D WIN32 /Fo$(OBJDIR)\ /Fd$(OBJDIR)\ /FD /I ../3rdParty/libpng /I ../3rdParty/libpng/Windows $(EXTRACFLAGS) /D EXPORT_3DS

!IF DEFINED(DEBUG)
CONFIG = Debug
CPPGLOBAL = $(CPPGLOBAL) /MTd /D _DEBUG
RFLAGS = $(RFLAGS) /D _DEBUG
!ELSE
CONFIG = Release
RFLAGS = $(RFLAGS) /d NDEBUG 
CPPGLOBAL = $(CPPGLOBAL) /MT /D NDEBUG
LFLAGS = $(LFLAGS) /nodefaultlib:libcmtd.lib
!ENDIF

!IF DEFINED(ANSI)
CPPGLOBAL = $(CPPGLOBAL) /D TC_NO_UNICODE
CONFIG = $(CONFIG)_ANSI
!ENDIF

!IF DEFINED(NO_BOOST)
CPPGLOBAL = $(CPPGLOBAL) /D _NO_BOOST
CONFIG = $(CONFIG)_NOBOOST
!ENDIF

!IF [ cl 2>&1 | findstr "x64" >NUL ] == 0
X64=1
!ENDIF

!IF $(NUMBER_OF_PROCESSORS) > 1
CPPGLOBAL = $(CPPGLOBAL) /MP
!ENDIF

!IF DEFINED(X64)
CONFIG= $(CONFIG)_X64
RFLAGS = $(RFLAGS) /D WIN64
!ENDIF

!IF DEFINED(POSTFIX)
CONFIG = $(CONFIG)$(POSTFIX)
!ENDIF

!IF DEFINED(LIBT)
!IF exist(..\make.inc)
OBJDIR = ..\Build\$(LIBT)\$(CONFIG)
!ENDIF
!IF exist(..\..\make.inc)
OBJDIR = ..\..\Build\$(LIBT)\$(CONFIG)
!ENDIF
!ELSE
!IF exist(make.inc)
OBJDIR = Build\$(CONFIG)
!ELSE
OBJDIR = ..\Build\$(CONFIG)
!IF DEFINED (LANG)
OBJDIR = ..\..\Build\$(LANG)
!ENDIF
!ENDIF
!ENDIF

!IF [ cl 2>&1 | findstr "13.0" >NUL ] == 0
SPECLIBS = tinyxml_STL-vc2003.lib libpng-vc6.lib
!ELSE
SPECLIBS =
!IF DEFINED(X64)
SPECLIBS = $(SPECLIBS) libpng-vs2005.lib libjpeg.lib
!ELSE
SPECLIBS = $(SPECLIBS) libpng-vc2005.lib libjpeg-vc2005.lib
!ENDIF
!ENDIF

MAKE = nmake /nologo

!IF DEFINED(LIBT)
all:	$(OBJDIR) $(OBJDIR)\$(LIBT).lib
!ENDIF

!IF DEFINED(LANG)

COBJS = 
CCOBJS = $(OBJDIR)\$(LANG).obj 

all: $(OBJDIR) $(OBJDIR)\LDView-$(LANG).dll

$(OBJDIR)\LDView.res:	LDViewMessages.ini

$(OBJDIR)\LDView-$(LANG).dll:	$(CCOBJS) $(OBJDIR)\LDView.res 
	link version.lib /nologo /dll /incremental:no /pdb:$(OBJDIR)\LDView-$(LANG).pdb \
	$(LFLAGS) /out:$(OBJDIR)\LDView-$(LANG).dll /implib:$(OBJDIR)\LDView-$(LANG).lib \
	$(OBJDIR)\$(LANG).obj  $(OBJDIR)\LDView.res

!ENDIF

$(OBJDIR):
	if not exist $(OBJDIR) mkdir $(OBJDIR)

{}.cpp{$(OBJDIR)}.obj::
	$(CC) $(CPPFLAGS) $<

{}.c{$(OBJDIR)}.obj:
	$(CC) $(CPPFLAGS) $<

{}.rc{$(OBJDIR)}.res:
	$(RC) $(RFLAGS) /fo$*.res $<

!IF DEFINED(LIBT)
$(OBJDIR)\$(LIBT).lib:  $(OBJDIR) $(CCOBJS) $(COBJS)
	link -lib -nologo $(LFLAGS) -out:$(OBJDIR)\$(LIBT).lib $(CCOBJS) $(COBJS)
!ENDIF

HCW=$(PROGRAMFILES)\Help Workshop\hcw.exe

clean:
!IF EXIST($(OBJDIR))
	-del $(OBJDIR)\*.obj 
	-del $(OBJDIR)\*.lib 
	-del $(OBJDIR)\*.exe 
	-del $(OBJDIR)\*.pch 
	-del $(OBJDIR)\*.idb 
	-del $(OBJDIR)\*.res 
	-del $(OBJDIR)\*.dll 
	-del $(OBJDIR)\*.exp 
	-del $(OBJDIR)\*.pdb
	-rmdir $(OBJDIR)
!endif
	
ansi:	
	set NO_BOOST=
	set ANSI=1
	set DEBUG=
	make

no_boost:	
	set NO_BOOST=1
	set ANSI=
	set DEBUG=
	make

ansi_no_boost:
	set NO_BOOST=1
	set ANSI=1
	set DEBUG=
	make

debugbuild:
	set NO_BOOST=
	set ANSI=
	set DEBUG=1
	make

debug_ansi:
	set DEBUG=1
	set ANSI=1
	set NO_BOOST=1
	make
	
