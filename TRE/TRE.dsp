# Microsoft Developer Studio Project File - Name="TRE" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TRE - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TRE.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TRE.mak" CFG="TRE - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TRE - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "TRE - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
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
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\TRE\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "..\include" /I "..\gl2ps" /I ".." /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
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
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\TRE\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../include" /I ".." /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
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
# PROP Output_Dir "..\Build\PartialDebug-VC6"
# PROP Intermediate_Dir "..\Build\TRE\PartialDebug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /GX /O2 /I ".." /I "..\include" /I "..\gl2ps" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "..\include" /I "..\gl2ps" /I ".." /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TRE - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TRE___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "TRE___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "..\Build\TRE\DebugNoBoost-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../include" /I ".." /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../include" /I ".." /I "../boost/include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TRE - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TRE___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "TRE___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TRE___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "TRE___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I "..\include" /I "..\gl2ps" /I ".." /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /YX /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "..\include" /I "..\gl2ps" /I ".." /I "../boost/include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_TC_STATIC" /D "_NO_BOOST" /YX /FD /G7 /c
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
# Name "TRE - Win32 DebugNoBoost"
# Name "TRE - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\TREColoredShapeGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TREGLExtensions.cpp
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

SOURCE=.\TRETexmappedShapeGroup.cpp
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

SOURCE=.\TREColoredShapeGroup.h
# End Source File
# Begin Source File

SOURCE=.\TREGL.h
# End Source File
# Begin Source File

SOURCE=.\TREGLExtensions.h
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

SOURCE=.\TREVertexArray.h
# End Source File
# Begin Source File

SOURCE=.\TREVertexKey.h
# End Source File
# Begin Source File

SOURCE=.\TREVertexStore.h
# End Source File
# End Group
# End Target
# End Project
