# Microsoft Developer Studio Project File - Name="LDLoader" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LDLoader - Win32 PartialDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDLoader.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDLoader.mak" CFG="LDLoader - Win32 PartialDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDLoader - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LDLoader - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I ".." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
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
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
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
# PROP Output_Dir "PartialDebug"
# PROP Intermediate_Dir "PartialDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
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
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\LDLActionLine.cpp
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

SOURCE=.\LDLFileLine.cpp
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

SOURCE=.\LDLFileLine.h
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
