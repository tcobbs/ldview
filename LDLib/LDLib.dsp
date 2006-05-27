# Microsoft Developer Studio Project File - Name="LDLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LDLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDLib.mak" CFG="LDLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LDLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "./boost/include" /I "../" /I "../include" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_LOG_PERFORMANCE" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLib - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDLib___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "LDLib___Win32_PartialDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PartialDebug"
# PROP Intermediate_Dir "PartialDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /I "../" /I "../include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "./boost/include" /I "../" /I "../include" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
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

# Name "LDLib - Win32 Release"
# Name "LDLib - Win32 Debug"
# Name "LDLib - Win32 PartialDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LDLibraryUpdateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdater.cpp

!IF  "$(CFG)" == "LDLib - Win32 Release"

# ADD CPP /W4
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "LDLib - Win32 Debug"

# ADD CPP /W3
# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "LDLib - Win32 PartialDebug"

# ADD CPP /W3
# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LDLPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\LDModelParser.cpp
# End Source File
# Begin Source File

SOURCE=.\LDrawModelViewer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LDLibraryUpdateInfo.h
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdater.h
# End Source File
# Begin Source File

SOURCE=.\LDLPreferences.h
# End Source File
# Begin Source File

SOURCE=.\LDLUserDefaultsKeys.h
# End Source File
# Begin Source File

SOURCE=.\LDModelParser.h
# End Source File
# Begin Source File

SOURCE=.\LDrawModelViewer.h
# End Source File
# End Group
# End Target
# End Project
