# Microsoft Developer Studio Project File - Name="LDView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LDView - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDView.mak" CFG="LDView - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 PartialDebug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LDView - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W4 /O2 /I "." /I "./include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /FD /c
# SUBTRACT CPP /Fr /YX
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 UxTheme.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib /nologo /subsystem:windows /machine:I386 /libpath:".\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W4 /GX /ZI /Od /I "." /I "./include" /D "_DEBUG" /D "_LOG_PERFORMANCE" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /libpath:".\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDView___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "LDView___Win32_PartialDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\PartialDebug"
# PROP Intermediate_Dir ".\PartialDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "D:\MSDEV\PROJECTS\LDView" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "." /I "./include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib wsock32.lib winmm.lib TCFoundation\Release\TCFoundation.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CUI.lib LDLib.lib TCFoundation.lib UxTheme.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\lib" /libpath:".\TCFoundation\Release" /libpath:".\CUI\Release" /libpath:".\LDLib\Release"

!ENDIF 

# Begin Target

# Name "LDView - Win32 Release"
# Name "LDView - Win32 Debug"
# Name "LDView - Win32 PartialDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AppResources.rc
# End Source File
# Begin Source File

SOURCE=.\LDVExtensionsSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewMain.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SSConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\SSModelWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SSPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\SSPreview.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AppResources.h
# End Source File
# Begin Source File

SOURCE=.\LDVExtensionsSetup.h
# End Source File
# Begin Source File

SOURCE=.\LDViewPreferences.h
# End Source File
# Begin Source File

SOURCE=.\LDViewWindow.h
# End Source File
# Begin Source File

SOURCE=.\ModelLoader.h
# End Source File
# Begin Source File

SOURCE=.\ModelWindow.h
# End Source File
# Begin Source File

SOURCE=.\SSConfigure.h
# End Source File
# Begin Source File

SOURCE=.\SSModelWindow.h
# End Source File
# Begin Source File

SOURCE=.\SSPassword.h
# End Source File
# Begin Source File

SOURCE=.\SSPreview.h
# End Source File
# Begin Source File

SOURCE=.\UserDefaultsKeys.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Icons\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Icons\edt_up.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_color.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_color_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_split.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_split_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_determinant.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_determinant_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_fnf.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_fnf_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_info.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_info_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_opengl.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_opengl_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\examine.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\extra_dirs_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\flythrou.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LDViewIcon.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\screen_saver_preview.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\wide_ico.ico
# End Source File
# End Group
# Begin Group "Web Pages"

# PROP Default_Filter "html"
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\ChangeHistory.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\footer.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\FutureFeatures.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\header.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\index.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\index.new.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\LDView.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\Textures\SansSerif.fnt
# End Source File
# Begin Source File

SOURCE=.\Textures\StudLogo.png
# End Source File
# End Target
# End Project
