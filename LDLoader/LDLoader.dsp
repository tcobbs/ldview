# Microsoft Developer Studio Project File - Name="LDLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LDLoader - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDLoader.mak" CFG="LDLoader - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDLoader - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LDLoader - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\LDLoader\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I ".." /I "../boost/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLoader - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\LDLoader\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I ".." /I "../boost/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLoader - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LDLoader___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "LDLoader___Win32_PartialDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\PartialDebug-VC6"
# PROP Intermediate_Dir "..\Build\LDLoader\PartialDebug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /I "../boost/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLoader - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LDLoader___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "LDLoader___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "..\Build\LDLoader\DebugNoBoost-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I ".." /I "../boost/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I ".." /I "../boost/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LDLoader - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDLoader___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "LDLoader___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LDLoader___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "LDLoader___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I ".." /I "../boost/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I ".." /I "../boost/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /YX /FD /G7 /c
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

# Name "LDLoader - Win32 Release"
# Name "LDLoader - Win32 Debug"
# Name "LDLoader - Win32 PartialDebug"
# Name "LDLoader - Win32 DebugNoBoost"
# Name "LDLoader - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LDLActionLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLAutoCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLCommentLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLConditionalLineLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLEmptyLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLError.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLFacing.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLFileLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLFindFileAlert.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLLineLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLMainModel.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLModel.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLModelLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLPrimitiveCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLQuadLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLShapeLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLTriangleLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLUnknownLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDrawIni.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\LDLActionLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLAutoCamera.h
# End Source File
# Begin Source File

SOURCE=.\LDLCamera.h
# End Source File
# Begin Source File

SOURCE=.\LDLCommentLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLConditionalLineLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLEmptyLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLError.h
# End Source File
# Begin Source File

SOURCE=.\LDLFacing.h
# End Source File
# Begin Source File

SOURCE=.\LDLFileLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLFindFileAlert.h
# End Source File
# Begin Source File

SOURCE=.\LDLLineLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLMacros.h
# End Source File
# Begin Source File

SOURCE=.\LDLMainModel.h
# End Source File
# Begin Source File

SOURCE=.\LDLModel.h
# End Source File
# Begin Source File

SOURCE=.\LDLModelLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLPalette.h
# End Source File
# Begin Source File

SOURCE=.\LDLPrimitiveCheck.h
# End Source File
# Begin Source File

SOURCE=.\LDLQuadLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLShapeLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLTriangleLine.h
# End Source File
# Begin Source File

SOURCE=.\LDLUnknownLine.h
# End Source File
# Begin Source File

SOURCE=.\LDrawIni.h
# End Source File
# Begin Source File

SOURCE=.\LDrawInP.h
# End Source File
# End Group
# End Target
# End Project
