#!/bin/sh
export DEBIAN_FRONTEND=noninteractive
GITROOT=https://github.com/tcobbs/ldview

while test $# -gt 0 ; do
	case $1 in
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
		test -d /usr/share/ldraw || mkdir /usr/share/ldraw
		test -f /usr/share/ldraw/complete.zip || wget -nv https://library.ldraw.org/library/updates/complete.zip -O /usr/share/ldraw/complete.zip
		test -d /usr/share/ldraw/parts || unzip -q /usr/share/ldraw/complete.zip -d /usr/share
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
		test -n "$LDVIEW_BRANCH" && test $LDVIEW_BRANCH != master && cd ldview && git checkout $LDVIEW_BRANCH && cd ..
		LDVIEW=ldview
	fi
}

if [ -f /etc/centos-release -o -f /etc/oracle-release ] ; then
	dnf install -y git rpm-build which
	download
	dnf builddep -y --skip-unavailable $LDVIEW/QT/LDView.spec || true
elif [ -f /etc/almalinux-release ] ; then
	dnf install -y git dnf-plugins-core rpm-build hostname which
	download
	dnf builddep -y --skip-unavailable $LDVIEW/QT/LDView.spec || true
elif [ -f /etc/altlinux-release ] ; then
	apt-get update
	apt-get install -y git debhelper build-essential lsb-release rpm-build qt6-base-devel libpng-devel libjpeg-devel libminizip-devel libGLU-devel qt6-tools libEGL-devel libOSMesa-devel sudo
	download
elif [ -f /etc/fedora-release -o -f /etc/mageia-release ] ; then
	dnf install -y git rpmlint ccache dnf-plugins-core rpm-build wget
	download
	dnf builddep -y $LDVIEW/QT/LDView.spec || true
elif [ -f /etc/rocky-release ] ; then
	dnf install -y git dnf-plugins-core rpm-build
	download
	dnf builddep -y $LDVIEW/QT/LDView.spec
elif [ -f /etc/redhat-release ] ; then
	dnf install -y git rpm-build
	download
	dnf builddep -y $LDVIEW/QT/LDView.spec

elif [ -f /etc/debian_version ] ; then
	apt-get update
	apt-get install -y git lintian build-essential debhelper \
				ccache lsb-release
	download
	for pkg in `grep Build-Depends $LDVIEW/QT/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,` libtinyxml2-dev libgl2ps-dev ; do
		apt-get --no-install-recommends install -y $pkg
	done
elif [ -f /etc/mandriva-release ] ; then
	urpmi --auto git rpm-build
	download
	urpmi --auto --buildrequires $LDVIEW/QT/LDView.spec
elif [ -f /etc/arch-release ] ; then
	pacman -Suy --noconfirm
	pacman -Sy --noconfirm git sudo binutils fakeroot tinyxml2 awk file inetutils debugedit
	download
	pacman -S --noconfirm `grep depends $LDVIEW/QT/PKGBUILD | cut -f2 -d=|tr -d \'\(\)`
elif grep -q -e openSUSE /etc/os-release ; then
	zypper --non-interactive install git rpm-build rpmlint hostname
	download
	zypper --non-interactive install --force-resolution `rpmbuild --nobuild $LDVIEW/QT/LDView.spec 2>&1 | grep 'needed by'| awk ' {print $1}'`
elif [ -f /etc/alpine-release ] ; then
	apk add git g++ alpine-sdk sudo
	download
	apk add `grep depends $LDVIEW/QT/APKBUILD |cut -f2 -d=|tr -d \"\(\)`
fi
git config --global pull.rebase false
