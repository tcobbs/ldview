@echo off
if "%1%"  == "uninstall" goto uninstall
if not exist c:\temp\zlib132.zip curl -OJL --output-dir c:\temp https://zlib.net/zlib132.zip
if not exist c:\temp\zlib-1.3.2 PowerShell Expand-Archive -Path c:\temp\zlib132.zip -DestinationPath c:\temp
if not exist c:\temp\zlib-1.3.1.tar.gz curl -OJL --output-dir c:\temp https://zlib.net/fossils/zlib-1.3.1.tar.gz
if not exist c:\temp\zlib-1.3.1.tar 7z e -oc:\temp c:\temp\zlib-1.3.1.tar.gz
if not exist c:\temp\zlib-1.3.1 7z x -oc:\temp c:\temp\zlib-1.3.1.tar

copy /y \temp\zlib-1.3.1\contrib\vstudio\vc17\zlibstat.vcxproj zlib\zlib.vcxproj >nul
copy /y \temp\zlib-1.3.2\*.c       zlib	>nul
copy /y \temp\zlib-1.3.2\*.h       zlib	>nul
copy /y \temp\zlib-1.3.2\ChangeLog zlib	>nul
copy /y \temp\zlib-1.3.2\INDEX     zlib	>nul
copy /y \temp\zlib-1.3.2\LICENSE   zlib	>nul
copy /y \temp\zlib-1.3.2\README    zlib	>nul
cd zlib
powershell "(Get-Content zlib.vcxproj).Replace('..\..\..\', '') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('..\..\..;', '.;') | Set-Content zlib.vcxproj"

powershell "(Get-Content zlib.vcxproj).Replace('zlibstat.lib', 'zlib.lib') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('zlibstat.pch', 'zlib.pch') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('MultiThreadedDebugDLL', 'MultiThreadedDebug') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('MultiThreadedDLL', 'MultiThreaded') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('ZLIB_WINAPI;', '') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('x86\ZlibStat$(Configuration)\</OutDir>', '$(SolutionDir)Build\$(Configuration)\</OutDir>') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('x86\ZlibStat$(Configuration)\Tmp\</IntDir>', '$(SolutionDir)Build\$(ProjectName)\$(Configuration)-inter\</IntDir>') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('x64\ZlibStat$(Configuration)\</OutDir>', '$(SolutionDir)Build\$(Configuration)64\</OutDir>') | Set-Content zlib.vcxproj"
powershell "(Get-Content zlib.vcxproj).Replace('x64\ZlibStat$(Configuration)\Tmp\</IntDir>', '$(SolutionDir)Build\$(ProjectName)\$(Configuration)64-inter\</IntDir>') | Set-Content zlib.vcxproj"

powershell "(Get-Content zlib.vcxproj).Replace('v143', 'v142') | Set-Content zlib.vcxproj"
powershell -command "(Get-Content zlib.vcxproj -Raw) -replace '(?s)(Win32.*?)v142(.*?PropertyGroup)', '$1v141_xp$2' | Set-Content zlib.vcxproj"
powershell -command "(Get-Content zlib.vcxproj -Raw) -replace '(?s)\s+<ItemGroup>\s+</ItemGroup>', '' | Set-Content zlib.vcxproj"

powershell -command "(Get-Content zlib.vcxproj) |Where-Object {$_ -notmatch 'minizip'} | Set-Content zlib.vcxproj"
powershell -command "(Get-Content zlib.vcxproj) |Where-Object {$_ -notmatch 'zlib.rc'} | Set-Content zlib.vcxproj"
powershell -command "(Get-Content zlib.vcxproj) |Where-Object {$_ -notmatch 'zlibvc.def'} | Set-Content zlib.vcxproj"
cd ..\
goto end

:uninstall
cd ..
git checkout -- *
rem del c:\temp\zlib*.zip
rem rd /s /q c:\temp\zlib-1.3.1
git status
cd 3rdparty
:end
