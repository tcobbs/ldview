#!/bin/sh
export DEBIAN_FRONTEND=noninteractive
GITROOT=https://github.com/tcobbs/ldview

while [[ $# -gt 0 ]]; do
	case $1 in
		-noqt4)
			NOQT4=true
			shift
		;;
		-noqt5)
			NOQT5=true
			shift
		;;
		-noqt6)
			NOQT6=true
			shift
		;;
		*)
			shift
		;;
	esac
done

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
	elif [ `pwd` = /root/project ] ; then
		LDVIEW=`pwd`
	elif [ -f QT/LDView.pro ] ; then
		LDVIEW=`pwd`
	else
		test -d ldview || git clone $GITROOT
		LDVIEW=ldview
	fi
	cp -f $LDVIEW/QT/LDView.spec $LDVIEW/QT/LDView-qt5.spec
	cp -f $LDVIEW/QT/LDView.spec $LDVIEW/QT/LDView-qt6.spec
	sed 's/define qt5 0/define qt5 1/' -i $LDVIEW/QT/LDView-qt5.spec
	sed 's/define qt6 0/define qt6 1/' -i $LDVIEW/QT/LDView-qt6.spec
}

if [ -f /etc/centos-release -o -f /etc/oracle-release ] ; then
	yum install -y git rpm-build rpmlint which
	download
	if which yum-builddep >/dev/null 2>/dev/null ; then
		test "$NOQT4" = true || yum-builddep -y $LDVIEW/QT/LDView.spec
		test "$NOQT5" = true || yum-builddep -y $LDVIEW/QT/LDView-qt5.spec
	else
		test "$NOQT4" = true || yum install -y `rpmbuild --nobuild $LDVIEW/QT/LDView.spec 2>&1 | grep 'needed by'| awk ' {print $1}'` 
		test "$NOQT5" = true || yum install -y `rpmbuild --nobuild $LDVIEW/QT/LDView-qt5.spec 2>&1 | grep 'needed by'| awk ' {print $1}'` || true
	fi
elif [ -f /etc/fedora-release -o -f /etc/mageia-release ] ; then
	dnf install -y git rpmlint ccache dnf-plugins-core rpm-build
	download
	test "$NOQT4" = true || dnf builddep -y $LDVIEW/QT/LDView.spec
	test "$NOQT5" = true || dnf builddep -y $LDVIEW/QT/LDView-qt5.spec
	test "$NOQT6" = true || dnf builddep -y $LDVIEW/QT/LDView-qt6.spec || true
elif [ -f /etc/rocky-release ] ; then
	dnf install -y git rpmlint dnf-plugins-core rpm-build
	download
	test "$NOQT4" = true || dnf builddep -y $LDVIEW/QT/LDView.spec
	test "$NOQT5" = true || dnf builddep -y $LDVIEW/QT/LDView-qt5.spec
elif [ -f /etc/redhat-release ] ; then
	dnf install -y git rpm-build
	download
	dnf builddep -y $LDVIEW/QT/LDView-qt5.spec

elif [ -f /etc/debian_version ] ; then
	apt-get update
	apt-get install -y git lintian build-essential debhelper \
				ccache lsb-release
	download
	for pkg in `grep Build-Depends $LDVIEW/QT/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,` libtinyxml-dev libgl2ps-dev ; do
		apt-get --no-install-recommends install -y $pkg
	done
elif [ -f /etc/mandriva-release ] ; then
	urpmi --auto git rpm-build
	download
	urpmi --auto --buildrequires $LDVIEW/QT/LDView.spec
elif [ -f /etc/arch-release ] ; then
	pacman -Suy --noconfirm
	pacman -Sy --noconfirm git sudo binutils fakeroot tinyxml awk file inetutils
	download
	test "$NOQT5" = true || pacman -S --noconfirm `grep depends $LDVIEW/QT/PKGBUILD | cut -f2 -d=|tr -d \'\(\)`
	test "$NOQT6" = true || pacman -S --noconfirm `grep depends $LDVIEW/QT/PKGBUILD | cut -f2 -d=|tr -d \'\(\)|sed 's/qt5/qt6/g'` qt6-5compat
elif grep -q -e openSUSE /etc/os-release ; then
	zypper --non-interactive install git rpm-build rpmlint hostname
	download
	test "$NOQT4" = true || zypper --non-interactive install `rpmbuild --nobuild $LDVIEW/QT/LDView.spec 2>&1 | grep 'needed by'| awk ' {print $1}'`
	test "$NOQT5" = true || zypper --non-interactive install --force-resolution `rpmbuild --nobuild $LDVIEW/QT/LDView-qt5.spec 2>&1 | grep 'needed by'| awk ' {print $1}'`
elif [ -f /etc/alpine-release ] ; then
	apk add git g++ alpine-sdk sudo
	download
	apk add `grep depends $LDVIEW/QT/APKBUILD |cut -f2 -d=|tr -d \"\(\)`
fi
git config --global pull.rebase false
