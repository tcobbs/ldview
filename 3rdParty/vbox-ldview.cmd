@echo off

rem	Copyright by Peter Bartfai (pbartfai@stardust.hu)

set ZEROIZE=1
set USER=root
set PASSWORD=root123

rem    --------------------------
rem    No Changes below this line
rem    --------------------------

set EXEC=--username %USER% --password %PASSWORD% --verbose  --wait-stdout
set VBM=%PROGRAMFILES%\Oracle\VirtualBox\VBoxManage.exe

if not exist rpm mkdir rpm

if "x%1x" == "xx" (
	shift
	goto all
)

set VM=%1
call :Build
goto :EOF

:all

FOR /F %%V IN ('"%VBM%" list vms') DO (
"%VBM%" showvminfo %%V|findstr "Guest"|findstr "OS:"|findstr /i "fedora ubuntu linux hat debian" > NUL
IF NOT ERRORLEVEL 1 (set VM=%%V
		 call :Build
)
)

goto :EOF


:Build
echo %VM%
echo Attaching shared folder lego
"%VBM%" sharedfolder add %VM% --name lego --hostpath "%CD%"

"%VBM%" startvm %VM%

:ism
ping 127.0.0.1 -n 3 -w 1000 > nul
"%VBM%" showvminfo %VM% | findstr /B "Additions"|findstr level|find /c "2" > NUL
IF ERRORLEVEL 1 ( goto ism )

echo Checking/Installing developement packages
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c "if [ -f /etc/redhat-release -a ! -f /root/ldview-dependency-installed ] ; then for pkg in cvs make qt-devel gcc gcc-c++ rpm-build boost-devel mesa-libOSMesa-devel kdebase-devel libpng-devel libjpeg-turbo-devel tinyxml-devel ; do if ! rpm -q --quiet $pkg ; then yum install -y $pkg ; fi; done ; touch /root/ldview-dependency-installed ; fi"
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -l -c "if [ -d /root ] ; then cd /root ; fi ; mkdir -p lego;cd lego;cvs -q -z3 -d:pserver:anonymous@ldview.cvs.sourceforge.net/cvsroot/ldview co LDView;cd LDView/QT;if [ -x /usr/lib/qt4/bin/qmake ] ; then /usr/lib/qt4/bin/qmake ; else qmake ; fi ; true make all"
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -l -c "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/redhat-release ] ; then rm -f /root/rpmbuild/RPMS/*/*.rpm ; cd /root/lego/LDView/QT ; rpmbuild -bb LDView.spec ; mount -t vboxsf lego /mnt ; cp -f /root/rpmbuild/RPMS/*/*.rpm /mnt/rpm ; umount /mnt ; fi"
rem "%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -l -c "if [ -d /root ] ; then export HOME=/root ; fi ; if [ -f /etc/debian_version ] ; then cd /root/lego/LDView/QT ; ./makedeb ; fi"

"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c "if [ -f /etc/redhat-release ] ; then yum -y -x 'kernel*' update ; fi"
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c "if [ -f /etc/debian_version ] ; then apt-get -y upgrade ; fi"
if "%ZEROIZE%"=="1" (
echo Zero filling ...
"%VBM%" guestcontrol %VM% execute %EXEC% --image "/bin/sh" -- -c "dd if=/dev/zero of=/ttt ; rm -f /ttt; touch /root/zerofilled"
)
rem pause

"%VBM%" guestcontrol %VM% execute %EXEC% --image "/sbin/shutdown" -- -h now

:off
ping 127.0.0.1 -n 3 -w 1000 > nul
"%VBM%" showvminfo %VM% | findstr /B "State"|findstr powered|find /c "off" > NUL
IF ERRORLEVEL 1 ( goto off )
echo Removig shared folder lego
"%VBM%" sharedfolder remove %VM% --name lego

goto :EOF