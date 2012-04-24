@echo off

rem	Copyright by Peter Bartfai (pbartfai@stardust.hu)

set ZEROIZE=0
set USER=root
set PASSWORD=root123

rem set ENGINE=virtualbox
set ENGINE=vmware

rem    --------------------------
rem    No Changes below this line
rem    --------------------------

set EXEC=--username %USER% --password %PASSWORD% --verbose  --wait-stderr
set VBM=%PROGRAMFILES%\Oracle\VirtualBox\VBoxManage.exe

IF EXIST "%PROGRAMFILES%\VMWare\VMWare Workstation\vmrun.exe" SET VMRUN=%PROGRAMFILES%\VMWare\VMWare Workstation\vmrun.exe
IF EXIST "%PROGRAMFILES(X86)%\VMWare\VMWare Workstation\vmrun.exe" SET VMRUN=%PROGRAMFILES(X86)%\VMWare\VMWare Workstation\vmrun.exe

set OPT=-T ws -gu %USER% -gp %PASSWORD%


if not exist rpm mkdir rpm

if not exist deb mkdir deb

if "x%1x" == "xx" (
	shift
	goto all
)

set VM=%1
call :Build
goto :EOF

:all

if "%ENGINE%"=="vmware" goto vmwareall

FOR /F %%V IN ('"%VBM%" list vms') DO (
"%VBM%" showvminfo %%V|findstr "Guest"|findstr "OS:"|findstr /i "fedora ubuntu linux hat debian" > NUL
IF NOT ERRORLEVEL 1 (set VM=%%V
		 call :Build
)
)

goto :EOF

:vmwareall

for /F "delims=: tokens=1,2 " %%i IN ('findstr /s "guestOS" *.vmx') do (
echo %%j | findstr /i "fedora ubuntu linux hat debian" > NUL
IF NOT ERRORLEVEL 1 (
	set VM=%%i
	call :Build
)
)

goto :EOF


:Build
time /t
echo %VM%
echo Attaching shared folder lego

IF "%ENGINE%"=="virtualbox" (
	"%VBM%" sharedfolder add %VM% --name lego --hostpath "%CD%"

	"%VBM%" startvm %VM%

:ism
	ping 127.0.0.1 -n 3 -w 1000 > nul
	"%VBM%" showvminfo %VM% | findstr /B "Additions"|findstr level|find /c "2" > NUL
	IF ERRORLEVEL 1 ( goto ism )
)
IF "%ENGINE%"=="vmware" (
	"%VMRUN%" %OPT% start "%VM%"
	"%VMRUN%" %OPT% directoryExistsInGuest "%VM%" /usr
	"%VMRUN%" %OPT% enableSharedFolders "%VM%"
	"%VMRUN%" %OPT% addSharedFolder "%VM%" lego "%CD%"
)
echo Checking/Installing developement packages
call :RUN "if [ -f /etc/redhat-release -a ! -f /root/ldview-dependency-installed ] ; then for pkg in cvs make qt-devel gcc gcc-c++ rpm-build boost-devel mesa-libOSMesa-devel kdebase-devel libpng-devel libjpeg-turbo-devel tinyxml-devel ; do if ! rpm -q --quiet $pkg ; then yum install -y $pkg ; fi; done ; touch /root/ldview-dependency-installed ; fi"
call :RUN "if [ -f /etc/debian_version -a ! -f /root/ldview-dependency-installed ] ; then for pkg in gcc cvs make g++ libqt4-dev libboost-thread-dev libpng-dev libjpeg-dev libtinyxml-dev cmake kdelibs5-dev fakeroot lintian ; do dpkg-query -W --showformat='${Package;-30}\t${Status}\n' $pkg|grep -qv not-installed;if [ $? -eq 1 ]  ;then apt-get -y install $pkg;fi;done;touch /root/ldview-dependency-installed ; fi"
echo Checking out CVS repository ...
call :RUN "if [ -d /root ] ; then cd /root ; fi ; mkdir -p lego;cd lego;cvs -q -z3 -d:pserver:anonymous@ldview.cvs.sourceforge.net/cvsroot/ldview co LDView;cd LDView/QT;if [ -x /usr/lib/qt4/bin/qmake ] ; then /usr/lib/qt4/bin/qmake ; else qmake ; fi ; true make all"
echo Building the packages ...
call :RUN "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/redhat-release ] ; then rm -f /root/rpmbuild/RPMS/*/*.rpm ; cd /root/lego/LDView/QT ; rpmbuild -bb LDView.spec; if [ -d /mnt/hgfs/lego ] ; then DST=/mnt/hgfs/lego ; else mount -t vboxsf lego /mnt ; DST=/mnt ; fi ; cp -f /root/rpmbuild/RPMS/*/*.rpm $DST/rpm ; fi"
if "%ENGINE%"=="vmware" (
call :RUN  "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/debian_version ] ; then cd /root/lego/LDView/QT ; ./makedeb ; if [ -d /mnt/hgfs/lego ] ; then DST=/mnt/hgfs/lego ; else mount -t vboxsf lego /mnt ; DST=/mnt ; fi ; cp -f ldview*.deb $DST/deb ; fi"
)
echo Updating Linux ...
call :RUN "if [ -f /etc/redhat-release ] ; then yum -y -x 'kernel*' update ; fi"
call :RUN "if [ -f /etc/debian_version ] ; then apt-get -y upgrade ; fi"
if "%ZEROIZE%"=="1" (
echo Zero filling ...
call :RUN "dd if=/dev/zero of=/ttt ; rm -f /ttt; touch /root/zerofilled"
)
rem pause
echo Shutting down ...
IF "%ENGINE%"=="virtualbox" (
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/sbin/shutdown" -- -h now

:off
ping 127.0.0.1 -n 3 -w 1000 > nul
"%VBM%" showvminfo %VM% | findstr /B "State"|findstr powered|find /c "off" > NUL
IF ERRORLEVEL 1 ( goto off )
echo Removig shared folder lego
"%VBM%" sharedfolder remove %VM% --name lego
) 
IF "%ENGINE%"=="vmware" (
	echo Removig shared folder lego
	"%VMRUN%" %OPT% removeSharedFolder "%VM%" lego
	"%VMRUN%" %OPT% stop "%VM%" soft
)
time /t
goto :EOF

:RUN
IF "%ENGINE%"=="virtualbox" (
	"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c %1
) ELSE (
	"%VMRUN%" %OPT% runScriptInGuest %VM% /bin/sh %1
)
goto :EOF
