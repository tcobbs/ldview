# Microsoft Developer Studio Project File - Name="TCFoundation" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=TCFoundation - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "TCFoundation.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "TCFoundation.mak" CFG="TCFoundation - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TCFoundation - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "TCFoundation - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "TCFoundation - Win32 PartialDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "TCFoundation - Win32 DebugNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE "TCFoundation - Win32 ReleaseNoBoost" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "TCFoundation - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\Release-VC6"
# PROP Intermediate_Dir "..\Build\TCFoundation\Release-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../include/" /I "../../boost_1_35_0" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /I "../3rdParty/libpng" /I "../3rdParty/libpng/windows" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /YX /FD /G7 /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TCFoundation - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\Debug-VC6"
# PROP Intermediate_Dir "..\Build\TCFoundation\Debug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /I "../3rdParty/libpng" /I "../3rdParty/libpng/windows" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TCFoundation - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TCFoundation___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "TCFoundation___Win32_PartialDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Build\PartialDebug-VC6"
# PROP Intermediate_Dir "..\Build\TCFoundation\PartialDebug-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /O2 /I "../" /I "../include/" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /YX /FD /c
# SUBTRACT BASE CPP /u
# ADD CPP /nologo /W4 /GX /O2 /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /I "../3rdParty/libpng" /I "../3rdParty/libpng/windows" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /YX /FD /c
# SUBTRACT CPP /u
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TCFoundation - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "TCFoundation___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "TCFoundation___Win32_DebugNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "..\Build\TCFoundation\DebugNoBoost-VC6"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /FR /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /I "../3rdParty/libpng" /I "../3rdParty/libpng/windows" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /D "_NO_BOOST" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "TCFoundation - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "TCFoundation___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "TCFoundation___Win32_ReleaseNoBoost"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "TCFoundation___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "TCFoundation___Win32_ReleaseNoBoost"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /YX /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "../" /I "../include/" /I "../boost/include" /I "../3rdParty/libjpeg" /I "../3rdParty/libjpeg/windows" /I "../3rdParty/libpng" /I "../3rdParty/libpng/windows" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "_BUILDING_TCFOUNDATION_LIB" /D "_NO_BOOST" /YX /FD /G7 /c
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

# Name "TCFoundation - Win32 Release"
# Name "TCFoundation - Win32 Debug"
# Name "TCFoundation - Win32 PartialDebug"
# Name "TCFoundation - Win32 DebugNoBoost"
# Name "TCFoundation - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ConvertUTF.c
# End Source File
# Begin Source File

SOURCE=.\mystring.cpp
# End Source File
# Begin Source File

SOURCE=.\TCAlert.cpp
# End Source File
# Begin Source File

SOURCE=.\TCAlertManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TCArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TCAutoreleasePool.cpp
# End Source File
# Begin Source File

SOURCE=.\TCBinaryObjectTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TCBinaryTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TCBmpImageFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\TCDictionary.cpp
# End Source File
# Begin Source File

SOURCE=.\TCImage.cpp
# End Source File
# Begin Source File

SOURCE=.\TCImageFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\TCImageOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\TCJpegImageFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\TCJpegOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\TCLocalStrings.cpp
# End Source File
# Begin Source File

SOURCE=.\TCNetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\TCNetworkClient.cpp
# End Source File
# Begin Source File

SOURCE=.\TCObject.cpp
# End Source File
# Begin Source File

SOURCE=.\TCObjectArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TCObjectTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TCPngImageFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\TCProgressAlert.cpp
# End Source File
# Begin Source File

SOURCE=.\TCSortedStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TCStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\TCStringObjectTree.cpp
# End Source File
# Begin Source File

SOURCE=.\TCUnzip.cpp
# End Source File
# Begin Source File

SOURCE=.\TCUserDefaults.cpp
# End Source File
# Begin Source File

SOURCE=.\TCVector.cpp
# End Source File
# Begin Source File

SOURCE=.\TCWebClient.cpp
# SUBTRACT CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ConvertUTF.h
# End Source File
# Begin Source File

SOURCE=.\mystring.h
# End Source File
# Begin Source File

SOURCE=.\TCAlert.h
# End Source File
# Begin Source File

SOURCE=.\TCAlertManager.h
# End Source File
# Begin Source File

SOURCE=.\TCArray.h
# End Source File
# Begin Source File

SOURCE=.\TCAutoreleasePool.h
# End Source File
# Begin Source File

SOURCE=.\TCBinaryObjectTree.h
# End Source File
# Begin Source File

SOURCE=.\TCBinaryTree.h
# End Source File
# Begin Source File

SOURCE=.\TCBmpImageFormat.h
# End Source File
# Begin Source File

SOURCE=.\TCDefines.h
# End Source File
# Begin Source File

SOURCE=.\TCDictionary.h
# End Source File
# Begin Source File

SOURCE=.\TCImage.h
# End Source File
# Begin Source File

SOURCE=.\TCImageFormat.h
# End Source File
# Begin Source File

SOURCE=.\TCImageOptions.h
# End Source File
# Begin Source File

SOURCE=.\TCJpegImageFormat.h
# End Source File
# Begin Source File

SOURCE=.\TCJpegOptions.h
# End Source File
# Begin Source File

SOURCE=.\TCLocalStrings.h
# End Source File
# Begin Source File

SOURCE=.\TCMacros.h
# End Source File
# Begin Source File

SOURCE=.\TCNetwork.h
# End Source File
# Begin Source File

SOURCE=.\TCNetworkClient.h
# End Source File
# Begin Source File

SOURCE=.\TCObject.h
# End Source File
# Begin Source File

SOURCE=.\TCObjectArray.h
# End Source File
# Begin Source File

SOURCE=.\TCObjectTree.h
# End Source File
# Begin Source File

SOURCE=.\TCPngImageFormat.h
# End Source File
# Begin Source File

SOURCE=.\TCProgressAlert.h
# End Source File
# Begin Source File

SOURCE=.\TCSortedStringArray.h
# End Source File
# Begin Source File

SOURCE=.\TCStlIncludes.h
# End Source File
# Begin Source File

SOURCE=.\TCStringArray.h
# End Source File
# Begin Source File

SOURCE=.\TCStringObjectTree.h
# End Source File
# Begin Source File

SOURCE=.\TCTypedDictionary.h
# End Source File
# Begin Source File

SOURCE=.\TCTypedObjectArray.h
# End Source File
# Begin Source File

SOURCE=.\TCTypedPointerArray.h
# End Source File
# Begin Source File

SOURCE=.\TCTypedValueArray.h
# End Source File
# Begin Source File

SOURCE=.\TCUnzip.h
# End Source File
# Begin Source File

SOURCE=.\TCUserDefaults.h
# End Source File
# Begin Source File

SOURCE=.\TCVector.h
# End Source File
# Begin Source File

SOURCE=.\TCWebClient.h
# End Source File
# End Group
# End Target
# End Project
