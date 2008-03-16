RFLAGS=/l 0x409 
CPPGLOBAL=/nologo /W4 /EHsc /Ox /D WIN32 /Fo$(OBJDIR)\ /Fd$(OBJDIR)\ /FD /G7 $(EXTRACFLAGS)

!IF DEFINED(DEBUG)
OBJDIR = Debug
CPPGLOBAL = $(CPPGLOBAL) /MTd
!ELSE
OBJDIR = Release
RFLAGS = $(RFLAGS) /d NDEBUG 
CPPGLOBAL = $(CPPGLOBAL) /MT /D NDEBUG
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
	/machine:I386 /out:$(OBJDIR)\LDView-$(LANG).dll /implib:$(OBJDIR)\LDView-$(LANG).lib \
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
	link -lib -nologo -out:$(OBJDIR)\$(LIBT) $(CCOBJS) $(COBJS)
!ENDIF

HCW=$(PROGRAMFILES)\Help Workshop\hcw.exe

clean:
	cd $(OBJDIR)
	-del *.obj *.lib *.exe *.pch *.idb *.res *.dll *.exp
	cd ..
