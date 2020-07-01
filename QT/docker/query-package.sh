#!/bin/sh
ARCH=x86_64

if [ -f /etc/fedora-release -o -f /etc/centos-release ] ; then
	for pkg in qt5-qtbase-devel ; do
		dnf list -q $pkg.$ARCH| grep $ARCH >/tmp/pkginfo.$$
		PKGNAME=`awk ' {print $1}' /tmp/pkginfo.$$ |sed "s/\\.$ARCH//g" `
		PKGVER=`awk ' {print $2}' /tmp/pkginfo.$$`
		echo $PKGNAME-$PKGVER
		rm -f /tmp/pkginfo.$$
	done
elif [ -f /etc/debian_version ] ; then
	apt-get -q update >/dev/null
	for pkg in qtbase5-dev ; do
		apt show $pkg >/tmp/pkginfo.$$ 2>/dev/null
		PKGNAME=`grep ^Package: /tmp/pkginfo.$$|cut -f2 -d\ `
		PKGVER=`grep ^Version: /tmp/pkginfo.$$|cut -f2 -d\ `
		echo $PKGNAME-$PKGVER
		rm -f /tmp/pkginfo.$$
	done
elif [ -f /etc/mandriva-release -o -f /etc/mageia-release ] ; then
	for pkg in lib64qt5base5-devel ; do
		urpmq -f $pkg|sed "s/\.$ARCH//g"|sed 's/\.mga[0-9]*//g'
	done
elif [ -f /etc/arch-release ] ; then
	pacman -Sy --noprogressbar -q >/dev/null
	pacman -Sy --noconfirm grep >/dev/null
	for pkg in qt5-base ; do
		pacman  -Si $pkg >/tmp/pkginfo.$$
		PKGNAME=`grep ^Name /tmp/pkginfo.$$ | cut -f2 -d:|sed 's/ //g'`
		PKGVER=`grep ^Version /tmp/pkginfo.$$|cut -f2 -d:|sed 's/ //g'`
		echo $PKGNAME-$PKGVER
		rm -f /tmp/pkginfo.$$
	done
elif grep -q -e openSUSE /etc/os-release ; then
	for pkg in libqt5-qtbase-devel ; do
		zypper -q info $pkg >/tmp/pkginfo.$$
		echo $pkg-`grep ^Version /tmp/pkginfo.$$|cut -f2 -d:|sed 's/ //g'`
		rm -f /tmp/pkginfo.$$
	done
fi
