@echo off

rem	Copyright by Peter Bartfai (pbartfai@stardust.hu)
rem
rem	Usage: vbox-ldview.cmd [<VMname or path to VMX file>]
rem
rem	Running without argument will run the build process to all registered VirtualBox based
rem	machines if Virtualbox is installed and run on all VMWare Workstation based VM located under
rem	current directory.
rem
rem	The only argument can be the name of VirtualBox VM or path to VMWare VMX file.

set ZEROIZE=1
set USER=root
set PASSWORD=root123
set FORCEINSTALL=0

rem    --------------------------
rem    No Changes below this line
rem    --------------------------

set EXEC=--username %USER% --password %PASSWORD% --verbose  --wait-stderr --timeout 1800000
IF EXIST "%PROGRAMFILES%\Oracle\VirtualBox\VBoxManage.exe" set VBM=%PROGRAMFILES%\Oracle\VirtualBox\VBoxManage.exe

IF EXIST "%PROGRAMFILES%\VMWare\VMWare Workstation\vmrun.exe" SET VMRUN=%PROGRAMFILES%\VMWare\VMWare Workstation\vmrun.exe
IF EXIST "%PROGRAMFILES(X86)%\VMWare\VMWare Workstation\vmrun.exe" SET VMRUN=%PROGRAMFILES(X86)%\VMWare\VMWare Workstation\vmrun.exe
IF EXIST "%PROGRAMFILES%\VMware\VMware VIX\vmrun.exe" SET VMRUN=%PROGRAMFILES%\VMware\VMware VIX\vmrun.exe
IF EXIST "%PROGRAMFILES(X86)%\VMware\VMware VIX\vmrun.exe" SET VMRUN=%PROGRAMFILES(X86)%\VMware\VMware VIX\vmrun.exe

set OPT=-T ws -gu %USER% -gp %PASSWORD%


if not exist rpm mkdir rpm

if not exist deb mkdir deb

if "x%1x" == "xx" (
	shift
	goto all
)

set VM=%1
echo %VM%
echo %VM% | findstr /i ".vmx" > NUL
IF ERRORLEVEL 1 goto singlevbox
set ENGINE=vmware
if "x%VMRUN%x" == "xx" goto :EOF
goto single


:singlevbox
set ENGINE=virtualbox
if "x%VBM%x" == "xx"  goto :EOF

:single
call :Build
goto :EOF

:all

if "x%VBM%x" == "xx"  goto vmwareall 

set ENGINE=virtualbox

FOR /F %%V IN ('"%VBM%" list vms') DO (
"%VBM%" showvminfo %%V|findstr "Guest"|findstr "OS:"|findstr /i "fedora ubuntu linux hat debian rhel centos" > NUL
IF NOT ERRORLEVEL 1 (set VM=%%V
		 call :Build
)
)

:vmwareall

if "x%VMRUN%x" == "xx" goto :EOF
set ENGINE=vmware
findstr /s "guestOS" *.vmx|findstr /v "distroName"|sort>vmlist.txt
for /F "delims=: tokens=1,2 " %%i IN (vmlist.txt) do (
echo %%j | findstr /i "fedora ubuntu linux hat debian centos rhel mandriva mandrake sles suse" > NUL
IF NOT ERRORLEVEL 1 (
	set VM=%%i
	call :Build
)
)
del vmlist.txt
goto :EOF


:Build
time /t
echo %VM%
for /f %%i in ("%vm%") do @set VMD=%%~pi
if exist %VMD%ldview.log del %VMD%ldview.log

echo Attaching shared folder lego

IF "%ENGINE%"=="virtualbox" (
	"%VBM%" sharedfolder add %VM% --name lego --hostpath "%CD%"

	"%VBM%" -v|findstr /r ^5 > NUL
	if ERRORLEVEL 1 goto vb4
	set VB5=1
:vb4
	"%VBM%" startvm %VM%
	set CNT=0
:ism
	ping 127.0.0.1 -n 3 -w 1000 > nul
	set /a CNT=%CNT% + 1
	IF "%CNT%" == "200"  goto poweroff
	"%VBM%" showvminfo %VM% | findstr /B "Additions"|findstr level|find /c "2" > NUL
	IF ERRORLEVEL 1  goto ism 
)
IF "%ENGINE%"=="vmware" (
	"%VMRUN%" %OPT% start "%VM%"
	"%VMRUN%" %OPT% directoryExistsInGuest "%VM%" /usr
	"%VMRUN%" %OPT% enableSharedFolders "%VM%"
	"%VMRUN%" %OPT% addSharedFolder "%VM%" lego "%CD%"
)
rem Maitenence
rem call :RUN "rm -f /root/zerofilled"
IF "%FORCEINSTALL%"=="1" (
call :RUN "rm -rf /root/ldview-dependency-installed /root/lego /root/rpm*/SOURCES/LDView /usr/src/redhat/SOURCES/LDView /usr/src/packages/SOURCES/LDView"
)

echo Checking/Installing developement packages
call :RUN "if which vmware-toolbox-cmd >/dev/null ; then vmware-toolbox-cmd -v > /tmp/ldview.log; vmware-toolbox-cmd upgrade status >> /tmp/ldview.log; fi"
call :RUN "if grep -q -e openSUSE /etc/os-release && test ! -f /root/ldview-dependency-installed ; then for pkg in git make libqt4-devel gcc gcc-c++ rpm-build boost-devel mesa-libOSMesa-devel libkde4-devel libpng-devel libjpeg-turbo-devel tinyxml-devel glu-devel rpmlint gl2ps-devel fdupes Mesa-devel ccache ; do if ! rpm -q --quiet $pkg ; then zypper --non-interactive install $pkg ; fi; done ; touch /root/ldview-dependency-installed ; fi"
call :RUN "if [ -f /etc/mandriva-release -a ! -f /root/ldview-dependency-installed ] ; then for pkg in git make libqt4-devel gcc gcc-c++ rpm-build boost-devel kdebase4-devel kdelibs4-devel cmake libpng-devel libjpeg-turbo-devel tinyxml-devel rpmlint ccache ; do if ! rpm -q --quiet $pkg ; then urpmi --auto $pkg ; fi; done ; touch /root/ldview-dependency-installed ;  mkdir -p /root/rpmbuild/{BUILD,BUILDROOT,RPMS/{i386,i486,i586,i686,x86_64},RPMS/noarch,SOURCES,SRPMS,SPECS,tmp} ; ln -sf /usr/lib/qt4/bin/lrelease /usr/bin ; fi"
call :RUN "if [ -f /etc/redhat-release -a ! -f /root/ldview-dependency-installed ] ; then for pkg in git make qt-devel qt4-devel gcc gcc-c++ rpm-build boost-devel mesa-libOSMesa-devel kdebase-devel libpng-devel libjpeg-turbo-devel tinyxml-devel rpmlint gl2ps-devel ccache ; do if ! rpm -q --quiet $pkg ; then yum install -y $pkg ; fi; done ; touch /root/ldview-dependency-installed ; fi"
call :RUN "if [ -f /etc/debian_version -a ! -f /root/ldview-dependency-installed ] ; then for pkg in gcc git make g++ libqt4-dev libboost-thread-dev libpng-dev libjpeg-dev libtinyxml-dev cmake kdelibs5-dev fakeroot lintian libgl2ps-dev build-essential libboost-dev debhelper libosmesa6-dev ccache qtbase5-dev qt5-qmake libqt5opengl5-dev ; do dpkg-query -W --showformat='${Package;-30}\t${Status}\n' $pkg|grep -qv not-installed;if [ $? -eq 1 ]  ;then apt-get -y install $pkg;fi;done;touch /root/ldview-dependency-installed ; fi"
call :RUN "if [ -f /etc/arch-release -a ! -f /root/ldview-dependency-installed ] ; then for pkg in gcc git make qt4 libpng libjpeg-turbo mesa-libgl ; do pacman -S --noconfirm $pkg ; done ; touch /root/ldview-dependency-installed ; fi"

echo Checking out GIT repository ...
call :RUN "if [ -d /root ] ; then cd /root ; fi ; mkdir -p lego;cd lego;if [ -d LDView ] ; then cd LDView ; git pull ; cd .. ; else git clone https://github.com/tcobbs/ldview LDView ; fi;cd LDView/QT;if [ -x /usr/lib/qt4/bin/qmake ] ; then /usr/lib/qt4/bin/qmake ; else qmake ; fi ; make clean"


echo Building the packages ...
call :RUN "if [ -d /root ] ; then export HOME=/root ; fi ; if test -f /etc/redhat-release || grep -q openSUSE /etc/os-release || test -f /etc/mandriva-release ; then rm -f /root/rpm*/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm ; if [ -f /etc/mandriva-release ] ; then touch /root/rpmbuild/SOURCES/LDView.tar.gz ; fi ; if grep -q 5\\. /etc/redhat-release ; then echo -e \\045centos_version 500\\n\\045dist .el5 >/root/.rpmmacros ; fi ; cd /root/lego/LDView/QT ; ./makerpm 2>&1 |tee /tmp/ldview.log >&2; if [ -d /mnt/hgfs/lego ] ; then DST=/mnt/hgfs/lego ; else mount -t vboxsf lego /mnt ; DST=/mnt ; fi ; for r in /root/rpm*/RPMS/*/ldview*.rpm /usr/src/redhat/RPMS/*/ldview*.rpm /usr/src/packages/RPMS/*/ldview*.rpm ; do if [ -f $r ] ; then cp -f $r $DST/rpm/ ; rpmlint $r 2>&1 |tee -a /tmp/ldview.log >&2 ; fi; done ; fi"
if "%ENGINE%"=="vmware" (
call :RUN  "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/debian_version ] ; then cd /root/lego/LDView/QT ; rm -f ldview*.deb ; ./makedeb -qt5 2>&1 |tee /tmp/ldview.log >&2; if [ -d /mnt/hgfs/lego ] ; then DST=/mnt/hgfs/lego ; else mount -t vboxsf lego /mnt ; DST=/mnt ; fi ; cp -f ldview*.deb $DST/deb ; fi"
)
call :RUN  "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/arch-release ] ; then cd /root/lego ; if [ -f ldview/QT/Makefile ] ; then cd ldview/QT;make clean ; fi ;cd ../.. ; cd ldview/OSMesa/ ; make clean ; cd ../.. ; mkdir -p /tmp/ldview/ ; chown nobody /tmp/ldview ; tar cfz /tmp/ldview/ldview-git.tar.gz ldview ; cp -f ldview/QT/PKGBUILD /tmp/ldview/ ; cd /tmp/ldview/src/ldview/QT ; sudo -u nobody makepkg -ef; if [ -d /mnt/hgfs/lego ] ; then DST=/mnt/hgfs/lego ; else mount -t vboxsf lego /mnt ; DST=/mnt ; fi ; cp -f ldview*.xz $DST/deb ; fi"

echo Updating Linux ...
call :RUN "if grep -q openSUSE /etc/os-release ; then zypper --non-interactive patch --auto-agree-with-licenses 2>&1 |tee /tmp/ldview.log >&2; fi"
call :RUN "if [ -f /etc/redhat-release ] ; then yum -y -x 'kernel*' -x 'open-vm-tools*' update 2>&1 |tee /tmp/ldview.log >&2; fi"
call :RUN "if [ -f /etc/debian_version ] ; then apt-get -y upgrade 2>&1 |tee /tmp/ldview.log >&2; fi"
call :RUN "if [ -f /etc/arch-release ] ; then pacman -Suy --noconfirm 2>&1 |tee /tmp/ldview.log >&2 ; fi"

if "%ZEROIZE%"=="1" (
echo Zero filling ...
call :RUN "if ! ( find /root -name zerofilled -ctime -30 | grep -q zerofilled ) ; then if which ccache >/dev/null 2>/dev/null ; then ccache -cC ; fi ; if which vmware-toolbox-cmd >/dev/null ; then vmware-toolbox-cmd disk shrink / ; else dd if=/dev/zero of=/ttt ; rm -f /ttt; fi; touch /root/zerofilled; fi"
)
rem pause
:shutdown
rem waiting for VMware-Tools upgrade process to be completed
call :RUN "while ps -ef|grep -v grep|grep -q vmware-tools-upgrader ; do sleep 5 ; done"
echo Shutting down ...
IF "%ENGINE%"=="virtualbox" (
if "%VB5%"=="1" (
"%VBM%" controlvm %VM% acpipowerbutton
"%VBM%" controlvm %VM% poweroff
) else (
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/sbin/shutdown" -- -h now
)
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
if "%VB5%"=="1" (
	"%VBM%" guestcontrol %VM% run %EXEC% -- "/bin/sh" -c %1
) ELSE (
	"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c %1
)
) ELSE (
	"%VMRUN%" %OPT% runScriptInGuest %VM% /bin/sh %1
 	"%VMRUN%" %OPT% runScriptInGuest %VM% /bin/sh "cat /tmp/ldview.log |sed s'/$/\r/g' >/mnt/hgfs/lego/ldview.log ; rm -f /tmp/ldview.log"
	if exist ldview.log type ldview.log
	for /f %%i in ("%vm%") do @set VMD=%%~pi
	if exist ldview.log type ldview.log >>%VMD%\ldview.log
	if exist ldview.log del /f ldview.log

)
goto :EOF

:poweroff
IF "%ENGINE%"=="virtualbox" (
	"%VBM%" controlvm %VM% poweroff
) ELSE (
	"%VMRUN%" %OPT% stop "%VM%" hard
)

goto :EOF
