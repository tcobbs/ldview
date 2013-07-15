# Microsoft Developer Studio Project File - Name="LDExporter" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LDExporter - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDExporter.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDExporter.mak" CFG="LDExporter - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDExporter - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LDExporter - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDExporter - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "LDExporter - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LDExporter - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\LDExporter\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../boost/include" /I "../" /I "../include" /I "../3rdParty/tinyxml" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "TIXML_USE_STL" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDExporter - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\LDExporter\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /I "../3rdParty/tinyxml" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "TIXML_USE_STL" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDExporter - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LDExporter___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "LDExporter___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "LDExporter___Win32_DebugNoBoost"
# PROP Intermediate_Dir "LDExporter___Win32_DebugNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../boost/include" /I "../" /I "../include" /I "../3rdParty/tinyxml" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "TIXML_USE_STL" /D "_NO_BOOST" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDExporter - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDExporter___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "LDExporter___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LDExporter___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "LDExporter___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "../boost/include" /I "../" /I "../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../boost/include" /I "../" /I "../include" /I "../3rdParty/tinyxml" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "TIXML_USE_STL" /D "_NO_BOOST" /YX /FD /c
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

# Name "LDExporter - Win32 Release"
# Name "LDExporter - Win32 Debug"
# Name "LDExporter - Win32 DebugNoBoost"
# Name "LDExporter - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LDExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\LDExporterSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\LDPovExporter.cpp
# End Source File
# Begin Source File

SOURCE=.\LDStlExporter.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LDExporter.h
# End Source File
# Begin Source File

SOURCE=.\LDExporterSetting.h
# End Source File
# Begin Source File

SOURCE=.\LDPovExporter.h
# End Source File
# Begin Source File

SOURCE=.\LDPovExportUDKeys.h
# End Source File
# Begin Source File

SOURCE=.\LDStlExporter.h
# End Source File
# End Group
# End Target
# End Project
