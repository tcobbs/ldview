#
#
#	Common rules for building LDView on Windows systems
#
#
#DEBUG=yes
#ANSI=yes
#NO_BOOST=yes

RFLAGS=/l 0x409 
CPPGLOBAL=/nologo /W4 /EHsc /Ox /D WIN32 /Fo$(OBJDIR)\ /Fd$(OBJDIR)\ /FD /G7 $(EXTRACFLAGS) /D EXPORT_3DS

!IF DEFINED(X64)
LFLAGS=$(LFLAGS) /MACHINE:X64
!ELSE
LFLAGS=$(LFLAGS) /MACHINE:I386
!ENDIF

!IF DEFINED(DEBUG)
OBJDIR = Debug
CPPGLOBAL = $(CPPGLOBAL) /MTd /D _DEBUG
RFLAGS = $(RFLAGS) /D _DEBUG
!ELSE
OBJDIR = Release
RFLAGS = $(RFLAGS) /d NDEBUG 
CPPGLOBAL = $(CPPGLOBAL) /MT /D NDEBUG
!ENDIF

!IF DEFINED(ANSI)
CPPGLOBAL = $(CPPGLOBAL) /D TC_NO_UNICODE
OBJDIR = $(OBJDIR)_ANSI
!ENDIF

!IF DEFINED(NO_BOOST)
CPPGLOBAL = $(CPPGLOBAL) /D _NO_BOOST
OBJDIR = $(OBJDIR)_NOBOOST
!ENDIF

!IFDEF VCTOOLKITINSTALLDIR
SPECLIBS = tinyxml_STL-vc2003.lib libpng-vc6.lib
!ELSE
SPECLIBS = tinyxml_STL.lib libpng.lib
!ENDIF

MAKE = nmake /nologo

!IF DEFINED(LIBT)
all:	$(OBJDIR) $(OBJDIR)\$(LIBT)
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
$(OBJDIR)\$(LIBT):  $(OBJDIR) $(CCOBJS) $(COBJS)
	link -lib -nologo $(LFLAGS) -out:$(OBJDIR)\$(LIBT) $(CCOBJS) $(COBJS)
!ENDIF

HCW=$(PROGRAMFILES)\Help Workshop\hcw.exe

clean:
!IF EXIST($(OBJDIR))
	cd $(OBJDIR)
	-del *.obj *.lib *.exe *.pch *.idb *.res *.dll *.exp *.pdb
	cd ..
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
	
