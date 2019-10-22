#!/bin/sh

GITROOT=https://github.com/tcobbs/ldview

download (){
	if [ `pwd` = /root/lego ] ; then
		if [ ! -d ldraw ] ; then
			wget -nv http://www.ldraw.org/library/updates/complete.zip
			unzip -q complete.zip
			rm -f complete.zip
		fi
		test -d ldview || git clone $GITROOT
		LDVIEW=/root/lego/ldview
	elif [ `pwd` = /home/travis/build/tcobbs/ldview ] ; then
		LDVIEW=`pwd`
	elif [ `pwd` = /home/appveyor/projects/ldview ] ; then
		LDVIEW=`pwd`
	else
		test -d ldview || git clone $GITROOT
		LDVIEW=ldview
	fi
	cp -f $LDVIEW/QT/LDView.spec $LDVIEW/QT/LDView-qt5.spec
	sed 's/define qt5 0/define qt5 1/' -i $LDVIEW/QT/LDView-qt5.spec
}

if [ -f /etc/centos-release ] ; then
	yum install -y git rpm-build rpmlint which
	download
	if which yum-builddep >/dev/null 2>/dev/null ; then
		yum-builddep -y $LDVIEW/QT/LDView.spec
		yum-builddep -y $LDVIEW/QT/LDView-qt5.spec
	else
		yum install -y `rpmbuild --nobuild $LDVIEW/QT/LDView.spec 2>&1  | grep 'needed by'| awk ' {print $1}'` 
		yum install -y `rpmbuild --nobuild $LDVIEW/QT/LDView-qt5.spec 2>&1  | grep 'needed by'| awk ' {print $1}'` || true
	fi
elif [ -f /etc/fedora-release  -o -f /etc/mageia-release ] ; then
	dnf install -y git rpmlint ccache dnf-plugins-core rpm-build
	download
	dnf builddep -y $LDVIEW/QT/LDView.spec
	dnf builddep -y $LDVIEW/QT/LDView-qt5.spec
elif [ -f /etc/debian_version ] ; then
	apt-get update
	apt-get install -y git lintian build-essential debhelper \
			   ccache lsb-release
	download
	apt-get install -y `grep Build-Depends $LDVIEW/QT/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,` libtinyxml-dev libgl2ps-dev
elif [ -f /etc/mandriva-release ] ; then
	urpmi --auto git rpm-build
	download
	urpmi --auto --buildrequires $LDVIEW/QT/LDView.spec
elif [ -f /etc/arch-release ] ; then
	pacman -Sy --noconfirm git sudo binutils fakeroot tinyxml awk file inetutils
	download
	pacman -S --noconfirm `grep depends $LDVIEW/QT/PKGBUILD | cut -f2 -d=|tr -d \'\(\)`
elif grep -q -e openSUSE /etc/os-release ; then
	zypper --non-interactive install git rpm-build rpmlint hostname
	download
	zypper --non-interactive install `rpmbuild --nobuild $LDVIEW/QT/LDView.spec 2>&1  | grep 'needed by'| awk ' {print $1}'`
	zypper --non-interactive install --force-resolution `rpmbuild --nobuild $LDVIEW/QT/LDView-qt5.spec 2>&1  | grep 'needed by'| awk ' {print $1}'`
fi
