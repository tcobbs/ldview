# Microsoft Developer Studio Project File - Name="CUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CUI - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CUI.mak" CFG="CUI - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CUI - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CUI - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "CUI - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "CUI - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CUI - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\CUI\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CUI - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\CUI\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CUI - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CUI___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "CUI___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "..\Build\CUI\DebugNoBoost-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CUI - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "CUI___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "CUI___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "CUI___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "CUI___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D _WIN32_WINDOWS=0x0410 /D "_BUILDING_CUI_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /YX /FD /G7 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "CUI - Win32 Release"
# Name "CUI - Win32 Debug"
# Name "CUI - Win32 DebugNoBoost"
# Name "CUI - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CUIColorButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIOGLWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CUISubWindowInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIThemes.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\CUIWindowResizer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CUIColorButton.h
# End Source File
# Begin Source File

SOURCE=.\CUIDefines.h
# End Source File
# Begin Source File

SOURCE=.\CUIDialog.h
# End Source File
# Begin Source File

SOURCE=.\CUIOGLWindow.h
# End Source File
# Begin Source File

SOURCE=.\CUIPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CUISubWindowInfo.h
# End Source File
# Begin Source File

SOURCE=.\CUIThemes.h
# End Source File
# Begin Source File

SOURCE=.\CUIWindow.h
# End Source File
# Begin Source File

SOURCE=.\CUIWindowResizer.h
# End Source File
# End Group
# End Target
# End Project
