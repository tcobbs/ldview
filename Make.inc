CPPGLOBAL=/nologo /MT /W4 /EHsc /Ox /D NDEBUG /D WIN32 /FoRelease\ /FdRelease\ /FD /G7
RFLAGS=/l 0x409 /d NDEBUG
OBJDIR = Release

MAKE = nmake /nologo

!IF DEFINED(LIBT)
all:	$(OBJDIR) $(OBJDIR)\$(LIBT)
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
	-del *.obj *.lib *.exe *.pch *.idb *.res

