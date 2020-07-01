#!/bin/sh
ARCH=x86_64

if [ -f /etc/fedora-release -o -f /etc/centos-release ] ; then
	for pkg in qt5-qtbase-devel gcc gl2ps tinyxml zlib libpng-devel libjpeg-turbo-devel mesa-libOSMesa-devel ; do
		dnf list -q $pkg.$ARCH 2>/dev/null| grep $ARCH >/tmp/pkginfo.$$
		PKGNAME=`awk ' {print $1}' /tmp/pkginfo.$$ |sed "s/\\.$ARCH//g" `
		PKGVER=`awk ' {print $2}' /tmp/pkginfo.$$|sed 's/^[0-9]*://g'`
		echo -n $PKGNAME-$PKGVER,
		rm -f /tmp/pkginfo.$$
	done
	echo -n -e '\b'
	echo " "
elif [ -f /etc/debian_version ] ; then
	apt-get -q update >/dev/null
	for pkg in qtbase5-dev gcc libgl2ps-dev libtinyxml-dev zlib1g libpng-dev libjpeg-dev libosmesa6-dev ; do
		apt show $pkg >/tmp/pkginfo.$$ 2>/dev/null
		PKGNAME=`grep ^Package: /tmp/pkginfo.$$|cut -f2 -d\ `
		PKGVER=`grep ^Version: /tmp/pkginfo.$$|cut -f2 -d\ |sed 's/^[0-9]*://g'`
		echo -n $PKGNAME-$PKGVER,
		rm -f /tmp/pkginfo.$$
	done
	/bin/echo -e -n '\b'
	echo " "
elif [ -f /etc/mandriva-release -o -f /etc/mageia-release ] ; then
	for pkg in lib64qt5base5-devel gcc lib64gl2ps-devel lib64tinyxml-devel lib64zlib1 lib64png-devel lib64jpeg-devel lib64osmesa-devel ; do
		echo -n `urpmq -f $pkg 2>/dev/null|sed "s/\.$ARCH//g"|sed 's/\.mga[0-9]*//g'`,
	done
	echo -n -e '\b'
	echo " "
elif [ -f /etc/arch-release ] ; then
	pacman -Sy --noprogressbar -q >/dev/null
	pacman -Sy --noconfirm grep awk >/dev/null
	for pkg in qt5-base gcc gl2ps tinyxml zlib libpng libjpeg-turbo mesa ; do
		pacman  -Si $pkg >/tmp/pkginfo.$$ 2>/dev/null
		PKGNAME=`grep ^Name /tmp/pkginfo.$$ | cut -f2 -d:|sed 's/ //g'`
		PKGVER=`grep ^Version /tmp/pkginfo.$$|awk ' {print $3}'|sed 's/^[0-9]*://g'`
		echo -n $PKGNAME-$PKGVER,
		rm -f /tmp/pkginfo.$$
	done
	echo -n -e '\b'
	echo " "
elif grep -q -e openSUSE /etc/os-release ; then
	for pkg in libqt5-qtbase-devel gcc gl2ps-devel tinyxml-devel zlib libpng16-compat-devel libjpeg8-devel Mesa-devel ; do
		zypper -q info $pkg >/tmp/pkginfo.$$
		echo -n $pkg-`grep ^Version /tmp/pkginfo.$$|cut -f2 -d:|sed 's/ //g'`,
		rm -f /tmp/pkginfo.$$
	done
	echo -n -e '\b'
	echo " "
fi
