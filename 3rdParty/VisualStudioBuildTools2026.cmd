@echo off

if not exist "%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
winget install Microsoft.VisualStudio.BuildTools --override ^"--wait --passive ^
--add Microsoft.Component.MSBuild ^
--add Microsoft.VisualStudio.Workload.VCTools ^
--add Microsoft.VisualStudio.Component.WinXP ^
--add Microsoft.VisualStudio.Component.VC.v141.x86.x64 ^
--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
--add Microsoft.VisualStudio.Component.VC.14.29.16.11.x86.x64 ^
--add Microsoft.VisualStudio.Component.Windows11SDK.26100 ^
--add Microsoft.VisualStudio.Component.VC.ATL ^
--add Microsoft.VisualStudio.Component.VC.v141.ATL ^
--add Microsoft.VisualStudio.Component.VC.14.29.16.11.ATL"
)
