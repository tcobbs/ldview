# Microsoft Developer Studio Project File - Name="TRE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TRE - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TRE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TRE.mak" CFG="TRE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TRE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TRE - Win32 Release"

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
# ADD CPP /nologo /W4 /GX /O2 /I ".." /I "..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TRE - Win32 Debug"

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
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I ".." /I "../include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TRE - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TRE___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "TRE___Win32_PartialDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PartialDebug"
# PROP Intermediate_Dir "PartialDebug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I ".." /I "..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I ".." /I "..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
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

# Name "TRE - Win32 Release"
# Name "TRE - Win32 Debug"
# Name "TRE - Win32 PartialDebug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\TRECamera.cpp
# End Source File
# Begin Source File

SOURCE=.\TREColoredShapeGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TREFacing.cpp
# End Source File
# Begin Source File

SOURCE=.\TREMainModel.cpp
# End Source File
# Begin Source File

SOURCE=.\TREModel.cpp
# End Source File
# Begin Source File

SOURCE=.\TREShapeGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TRESmoother.cpp
# End Source File
# Begin Source File

SOURCE=.\TRESubModel.cpp
# End Source File
# Begin Source File

SOURCE=.\TRETransShapeGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TREVertexArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TREVertexStore.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\TRECamera.h
# End Source File
# Begin Source File

SOURCE=.\TREColoredShapeGroup.h
# End Source File
# Begin Source File

SOURCE=.\TREFacing.h
# End Source File
# Begin Source File

SOURCE=.\TREGL.h
# End Source File
# Begin Source File

SOURCE=.\TREMainModel.h
# End Source File
# Begin Source File

SOURCE=.\TREModel.h
# End Source File
# Begin Source File

SOURCE=.\TREShapeGroup.h
# End Source File
# Begin Source File

SOURCE=.\TRESmoother.h
# End Source File
# Begin Source File

SOURCE=.\TRESubModel.h
# End Source File
# Begin Source File

SOURCE=.\TRETransShapeGroup.h
# End Source File
# Begin Source File

SOURCE=.\TREVectorKey.h
# End Source File
# Begin Source File

SOURCE=.\TREVertexArray.h
# End Source File
# Begin Source File

SOURCE=.\TREVertexStore.h
# End Source File
# End Group
# End Target
# End Project
