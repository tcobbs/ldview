CPPGLOBAL=/nologo /MT /W4 /EHsc /Ox /D NDEBUG /D WIN32 /FoRelease\ /FdRelease\ /FD /G7 $(EXTRACFLAGS)
RFLAGS=/l 0x409 /d NDEBUG 
OBJDIR = Release

MAKE = nmake /nologo

!IF DEFINED(LIBT)
all:	$(OBJDIR) $(OBJDIR)\$(LIBT)
!ENDIF

!IF DEFINED(LANG)

COBJS = 
CCOBJS = $(OBJDIR)\$(LANG).obj 

all: $(OBJDIR) $(OBJDIR)\LDView-$(LANG).dll

$(OBJDIR)\Resources.res:	LDViewMessages.ini

$(OBJDIR)\LDView-$(LANG).dll:	$(CCOBJS) $(OBJDIR)\Resources.res 
	link version.lib /nologo /dll /incremental:no /pdb:$(OBJDIR)\LDView-$(LANG).pdb \
	/machine:I386 /out:$(OBJDIR)\LDView-$(LANG).dll /implib:$(OBJDIR)\LDView-$(LANG).lib \
	$(OBJDIR)\$(LANG).obj  $(OBJDIR)\Resources.res

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

clean:
	cd $(OBJDIR)
	-del *.obj *.lib *.exe *.pch *.idb *.res *.dll *.exp

