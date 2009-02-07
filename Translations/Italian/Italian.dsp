# Microsoft Developer Studio Project File - Name="Italian" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Italian - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Italian.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Italian.mak" CFG="Italian - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Italian - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Italian - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Italian - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITALIAN_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W4 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITALIAN_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\.." /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 version.lib /nologo /dll /machine:I386 /out:"Release/LDView-Italian.dll"

!ELSEIF  "$(CFG)" == "Italian - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITALIAN_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ITALIAN_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\.." /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /dll /debug /machine:I386 /out:"Debug/LDView-Italian.dll" /pdbtype:sept
# Begin Custom Build
TargetPath=.\Debug\LDView-Italian.dll
TargetName=LDView-Italian
InputPath=.\Debug\LDView-Italian.dll
SOURCE="$(InputPath)"

"..\Debug\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy "$(TargetPath)" ..\Debug

# End Custom Build

!ENDIF 

# Begin Target

# Name "Italian - Win32 Release"
# Name "Italian - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Italian.cpp
# End Source File
# Begin Source File

SOURCE=.\LDView.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icons\error_colinear.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_colinear_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_mask.bmp
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

SOURCE=.\Icons\error_matching_points.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_matching_points_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_matrix.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_matrix_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_non_flat_quad.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_non_flat_quad_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_vertex_order.bmp
# End Source File
# Begin Source File

SOURCE=.\icons\error_vertex_order_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\examine.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\flythrou.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LDrawIcon.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LDViewIcon.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleLL.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleLM.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleLR.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleML.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleMM.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleMR.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleUL.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleUM.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\LightAngleUR.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\MPDIcon.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\LDExporter\LDExportMessages.ini
# End Source File
# Begin Source File

SOURCE=.\LDView.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\LDViewMessages.ini
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Textures\SansSerif.fnt
# End Source File
# Begin Source File

SOURCE=.\Icons\screen_saver_preview.png
# End Source File
# Begin Source File

SOURCE=.\Textures\StudLogo.png
# End Source File
# End Target
# End Project
