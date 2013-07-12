# Microsoft Developer Studio Project File - Name="LDLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LDLib - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDLib.mak" CFG="LDLib - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLib - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
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
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\LDLib\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "./boost/include" /I "../" /I "../include" /I "../gl2ps" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
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
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\LDLib\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /I "../gl2ps" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_LOG_PERFORMANCE" /FR /YX /FD /GZ /c
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
# PROP Output_Dir "..\Build\PartialDebug-VC6"
# PROP Intermediate_Dir "..\Build\LDLib\PartialDebug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /I "../" /I "../include" /I "../gl2ps" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "./boost/include" /I "../" /I "../include" /I "../gl2ps" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLib - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LDLib___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "LDLib___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "..\Build\LDLib\DebugNoBoost-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /I "../gl2ps" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_LOG_PERFORMANCE" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /I "../gl2ps" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_LOG_PERFORMANCE" /D "_NO_BOOST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLib - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDLib___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "LDLib___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LDLib___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "LDLib___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I "./boost/include" /I "../" /I "../include" /I "../gl2ps" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "./boost/include" /I "../" /I "../include" /I "../gl2ps" /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /YX /FD /G7 /c
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
# Name "LDLib - Win32 DebugNoBoost"
# Name "LDLib - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LDHtmlInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\LDInputHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdateInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdater.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\LDModelParser.cpp
# End Source File
# Begin Source File

SOURCE=.\LDModelTree.cpp
# End Source File
# Begin Source File

SOURCE=.\LDObiInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\LDPartCount.cpp
# End Source File
# Begin Source File

SOURCE=.\LDPartsList.cpp
# End Source File
# Begin Source File

SOURCE=.\LDPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\LDrawModelViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\LDSnapshotTaker.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewPoint.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LDHtmlInventory.h
# End Source File
# Begin Source File

SOURCE=.\LDInputHandler.h
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdateInfo.h
# End Source File
# Begin Source File

SOURCE=.\LDLibraryUpdater.h
# End Source File
# Begin Source File

SOURCE=.\LDModelParser.h
# End Source File
# Begin Source File

SOURCE=.\LDModelTree.h
# End Source File
# Begin Source File

SOURCE=.\LDObiInfo.h
# End Source File
# Begin Source File

SOURCE=.\LDPartCount.h
# End Source File
# Begin Source File

SOURCE=.\LDPartsList.h
# End Source File
# Begin Source File

SOURCE=.\LDPreferences.h
# End Source File
# Begin Source File

SOURCE=.\LDrawModelViewer.h
# End Source File
# Begin Source File

SOURCE=.\LDSnapshotTaker.h
# End Source File
# Begin Source File

SOURCE=.\LDUserDefaultsKeys.h
# End Source File
# Begin Source File

SOURCE=.\LDViewPoint.h
# End Source File
# End Group
# End Target
# End Project
