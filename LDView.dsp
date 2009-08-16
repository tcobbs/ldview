# Microsoft Developer Studio Project File - Name="LDView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=LDView - Win32 DebugNoBoost
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LDView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LDView.mak" CFG="LDView - Win32 DebugNoBoost"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LDView - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 PartialDebug" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 DebugNoBoost" (based on "Win32 (x86) Application")
!MESSAGE "LDView - Win32 ReleaseNoBoost" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LDView - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\Release-VC6"
# PROP Intermediate_Dir "Release-VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "." /I "./include" /I "./boost/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /FD /G7 /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "LDVIEW_APP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng-VC6.lib zlib.lib version.lib unzip32.lib tinyxml_STL-VC6.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /libpath:".\lib" /libpath:".\boost\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\Debug-VC6"
# PROP Intermediate_Dir "Debug-VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W4 /GX /ZI /Od /I "." /I "./include" /I "./boost/include" /D "_DEBUG" /D "_LOG_PERFORMANCE" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "LDVIEW_APP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng-VC6.lib zlib.lib version.lib unzip32.lib libjpeg.lib tinyxmld_STL-VC6.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /libpath:".\lib" /libpath:".\boost\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 PartialDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDView___Win32_PartialDebug"
# PROP BASE Intermediate_Dir "LDView___Win32_PartialDebug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Build\PartialDebug-VC6"
# PROP Intermediate_Dir "PartialDebug-VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /I "D:\MSDEV\PROJECTS\LDView" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "." /I "./include" /I "./boost/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "LDVIEW_APP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib wsock32.lib winmm.lib TCFoundation\Release\TCFoundation.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 CUI.lib LDLib.lib TCFoundation.lib UxTheme.lib libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib unzip32.lib tinyxmld_STL-VC6.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:".\TCFoundation\PartialDebug" /libpath:".\CUI\Release" /libpath:".\LDLib\PartialDebug" /libpath:".\LDLoader\PartialDebug" /libpath:".\lib" /libpath:".\boost\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 DebugNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LDView___Win32_DebugNoBoost"
# PROP BASE Intermediate_Dir "LDView___Win32_DebugNoBoost"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Build\DebugNoBoost-VC6"
# PROP Intermediate_Dir "DebugNoBoost-VC6"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W4 /GX /ZI /Od /I "." /I "./include" /I "./boost/include" /D "_DEBUG" /D "_LOG_PERFORMANCE" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /FR /FD /c
# ADD CPP /nologo /MTd /W4 /GX /ZI /Od /I "." /I "./include" /I "./boost/include" /D "_DEBUG" /D "_LOG_PERFORMANCE" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /D "_NO_BOOST" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "LDVIEW_APP"
# ADD RSC /l 0x409 /d "_DEBUG" /d "LDVIEW_APP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib unzip32.lib libjpeg.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /libpath:".\lib" /libpath:".\boost\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng-VC6.lib zlib.lib version.lib unzip32.lib libjpeg.lib tinyxmld_STL-VC6.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /libpath:".\lib" /libpath:".\boost\lib"

!ELSEIF  "$(CFG)" == "LDView - Win32 ReleaseNoBoost"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LDView___Win32_ReleaseNoBoost"
# PROP BASE Intermediate_Dir "LDView___Win32_ReleaseNoBoost"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "LDView___Win32_ReleaseNoBoost"
# PROP Intermediate_Dir "LDView___Win32_ReleaseNoBoost"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W4 /GX /Ox /I "." /I "./include" /I "./boost/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /FD /G7 /c
# ADD CPP /nologo /MT /W4 /GX /Ox /I "." /I "./include" /I "./boost/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D _WIN32_WINDOWS=0x0410 /D "_TC_STATIC" /D "LDVIEW_APP" /D "_NO_BOOST" /FD /G7 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "LDVIEW_APP"
# ADD RSC /l 0x409 /d "NDEBUG" /d "LDVIEW_APP"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng.lib zlib.lib version.lib unzip32.lib tinyxml_STL-VC6.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /libpath:".\lib" /libpath:".\boost\lib"
# ADD LINK32 libjpeg.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib opengl32.lib glu32.lib ws2_32.lib winmm.lib shlwapi.lib comctl32.lib libpng-VC6.lib zlib.lib version.lib unzip32.lib tinyxml_STL-VC6.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /libpath:".\lib" /libpath:".\boost\lib"

!ENDIF 

# Begin Target

# Name "LDView - Win32 Release"
# Name "LDView - Win32 Debug"
# Name "LDView - Win32 PartialDebug"
# Name "LDView - Win32 DebugNoBoost"
# Name "LDView - Win32 ReleaseNoBoost"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\BoolOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\BoundingBoxDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EnumOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportOptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\FloatOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\JpegOptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LatLonDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LDLoader\LDLActionLine.cpp
# End Source File
# Begin Source File

SOURCE=.\LDVExtensionsSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\Help\LDView.hpj

!IF  "$(CFG)" == "LDView - Win32 Release"

USERDEP__LDVIE="Help\EffectsPrefs.rtf"	"Help\GeneralPrefs.rtf"	"Help\GeometryPrefs.rtf"	"Help\PrefSetsPrefs.rtf"	"Help\PrimitivesPrefs.rtf"	"Help\SaveSnapshot.rtf"	"Help\ScreenSaverPrefs.rtf"	"Help\UpdatesPrefs.rtf"	"Help\LDView.hpj"	
# Begin Custom Build - Compiling help project $(InputPath)
OutDir=.\Build\Release-VC6
InputPath=.\Help\LDView.hpj

"$(OutDir)\LDView.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hcrtf /x $(InputPath) 
	copy /y Help\LDView.hlp $(OutDir)\LDView.hlp > nul 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LDView - Win32 Debug"

USERDEP__LDVIE="Help\EffectsPrefs.rtf"	"Help\GeneralPrefs.rtf"	"Help\GeometryPrefs.rtf"	"Help\PrefSetsPrefs.rtf"	"Help\PrimitivesPrefs.rtf"	"Help\SaveSnapshot.rtf"	"Help\ScreenSaverPrefs.rtf"	"Help\UpdatesPrefs.rtf"	"Help\LDView.hpj"	
# Begin Custom Build - Compiling help project $(InputPath)
OutDir=.\Build\Debug-VC6
InputPath=.\Help\LDView.hpj

"$(OutDir)\LDView.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hcrtf /x $(InputPath) 
	copy /y Help\LDView.hlp $(OutDir)\LDView.hlp > nul 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LDView - Win32 PartialDebug"

!ELSEIF  "$(CFG)" == "LDView - Win32 DebugNoBoost"

USERDEP__LDVIE="Help\EffectsPrefs.rtf"	"Help\GeneralPrefs.rtf"	"Help\GeometryPrefs.rtf"	"Help\PrefSetsPrefs.rtf"	"Help\PrimitivesPrefs.rtf"	"Help\SaveSnapshot.rtf"	"Help\ScreenSaverPrefs.rtf"	"Help\UpdatesPrefs.rtf"	"Help\LDView.hpj"	
# Begin Custom Build - Compiling help project $(InputPath)
OutDir=.\Build\DebugNoBoost-VC6
InputPath=.\Help\LDView.hpj

"$(OutDir)\LDView.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hcrtf /x $(InputPath) 
	copy /y Help\LDView.hlp $(OutDir)\LDView.hlp > nul 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "LDView - Win32 ReleaseNoBoost"

USERDEP__LDVIE="Help\EffectsPrefs.rtf"	"Help\GeneralPrefs.rtf"	"Help\GeometryPrefs.rtf"	"Help\PrefSetsPrefs.rtf"	"Help\PrimitivesPrefs.rtf"	"Help\SaveSnapshot.rtf"	"Help\ScreenSaverPrefs.rtf"	"Help\UpdatesPrefs.rtf"	"Help\LDView.hpj"	
# Begin Custom Build - Compiling help project $(InputPath)
OutDir=.\LDView___Win32_ReleaseNoBoost
InputPath=.\Help\LDView.hpj

"$(OutDir)\LDView.hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	hcrtf /x $(InputPath) 
	copy /y Help\LDView.hlp $(OutDir)\LDView.hlp > nul 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LDView.rc
# End Source File
# Begin Source File

SOURCE=.\LDViewMain.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewPreferences.cpp
# End Source File
# Begin Source File

SOURCE=.\LDViewWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\LongOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelTreeDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\ModelWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\MpdDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsScroller.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\PartsListDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PathOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\SSConfigure.cpp
# End Source File
# Begin Source File

SOURCE=.\SSModelWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\SSPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\SSPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\StepDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StringOptionUI.cpp
# End Source File
# Begin Source File

SOURCE=.\TbButtonInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolbarStrip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\BoolOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\BoundingBoxDialog.h
# End Source File
# Begin Source File

SOURCE=.\EnumOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\ExportOptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\FloatOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\GroupOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\JpegOptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\LatLonDialog.h
# End Source File
# Begin Source File

SOURCE=.\LDVExtensionsSetup.h
# End Source File
# Begin Source File

SOURCE=.\LDViewPreferences.h
# End Source File
# Begin Source File

SOURCE=.\LDViewWindow.h
# End Source File
# Begin Source File

SOURCE=.\LongOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\ModelLoader.h
# End Source File
# Begin Source File

SOURCE=.\ModelTreeDialog.h
# End Source File
# Begin Source File

SOURCE=.\ModelWindow.h
# End Source File
# Begin Source File

SOURCE=.\MpdDialog.h
# End Source File
# Begin Source File

SOURCE=.\NumberOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\OptionsCanvas.h
# End Source File
# Begin Source File

SOURCE=.\OptionsScroller.h
# End Source File
# Begin Source File

SOURCE=.\OptionUI.h
# End Source File
# Begin Source File

SOURCE=.\PartsListDialog.h
# End Source File
# Begin Source File

SOURCE=.\PathOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SSConfigure.h
# End Source File
# Begin Source File

SOURCE=.\SSModelWindow.h
# End Source File
# Begin Source File

SOURCE=.\SSPassword.h
# End Source File
# Begin Source File

SOURCE=.\SSPreview.h
# End Source File
# Begin Source File

SOURCE=.\StepDialog.h
# End Source File
# Begin Source File

SOURCE=.\StringOptionUI.h
# End Source File
# Begin Source File

SOURCE=.\TbButtonInfo.h
# End Source File
# Begin Source File

SOURCE=.\ToolbarStrip.h
# End Source File
# Begin Source File

SOURCE=.\UserDefaultsKeys.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Icons\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\Icons\edt_up.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_colinear.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_colinear_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_color.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_color_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_split.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_concave_quad_split_mask.bmp
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

SOURCE=.\Icons\error_loop.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_loop_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_matching_points.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_matching_points_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_matrix.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_matrix_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_non_flat_quad.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_non_flat_quad_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_opengl.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_opengl_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_parse_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_vertex_order.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\error_vertex_order_mask.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\examine.ico
# End Source File
# Begin Source File

SOURCE=.\Icons\extra_dirs_toolbar.bmp
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

SOURCE=.\Icons\LightAngles.bmp
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
# Begin Source File

SOURCE=.\Icons\screen_saver_preview.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\step_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\Icons\wide_ico.ico
# End Source File
# End Group
# Begin Group "Web Pages"

# PROP Default_Filter "html"
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\ChangeHistory.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\footer.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\FutureFeatures.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\header.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\index.html
# End Source File
# Begin Source File

SOURCE=..\..\..\users\tcobbs\LDView\WebPages\index.new.html
# End Source File
# End Group
# Begin Source File

SOURCE=.\Build.txt
# End Source File
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

SOURCE=.\Textures\SansSerif.fnt
# End Source File
# Begin Source File

SOURCE=.\Icons\screen_saver_preview.png
# End Source File
# Begin Source File

SOURCE=.\Textures\StudLogo.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_all_cond.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_axes.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_bfc.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_cond_ctrl.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_edge.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_errors.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_examine.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_export.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_flat.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_fly.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_fullscreen.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_help.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_latlon.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_latlonrot.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_light.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_low_studs.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_model_bbox.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_model_tree.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_mpd.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_open.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_part_bbox.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_partslist.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_povcamera.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_preferences.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_primsubs.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_print.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_random.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_reload.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_seams.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_smooth.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_snapshot.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_step_first.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_step_last.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_step_next.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_step_prev.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_stud.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_topmost.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_trans_default.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_2_3rds.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_back.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_bottom.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_front.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_left.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_right.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_view_top.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_wire_cutaway.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_wireframe.png
# End Source File
# Begin Source File

SOURCE=.\Icons\toolbar_zoom2fit.png
# End Source File
# End Target
# End Project
