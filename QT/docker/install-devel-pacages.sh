#!/bin/sh

GITROOT=https://github.com/tcobbs/ldview

gitdownload (){
	git clone $GITROOT
	cp -f ldview/QT/LDView.spec ldview/QT/LDView-qt5.spec
	sed 's/define qt5 0/define qt5 1/' -i ldview/QT/LDView-qt5.spec
}

if [ -f /etc/centos-release ] ; then
	yum install -y git rpm-build rpmlint ccache
	gitdownload
	if which yum-builddep 2>/dev/null ; then
		yum-builddep -y ldview/QT/LDView.spec
		yum-builddep -y ldview/QT/LDView-qt5.spec
	else
		yum install -y `rpmbuild --nobuild ldview/QT/LDView.spec 2>&1  | grep 'needed by'| awk ' {print $1}'` 
#		yum install -y `rpmbuild --nobuild ldview/QT/LDView-qt5.spec 2>&1  | grep 'needed by'| awk ' {print $1}'`
	fi
elif [ -f /etc/fedora-release  -o -f /etc/mageia-release ] ; then
	dnf install -y git rpmlint ccache dnf-plugins-core rpm-build
	gitdownload
	dnf builddep -y ldview/QT/LDView.spec
	dnf builddep -y ldview/QT/LDView-qt5.spec
elif [ -f /etc/debian_version ] ; then
	apt-get update
	apt-get install -y git lintian build-essential debhelper \
			   ccache lsb-release
	gitdownload
	apt-get install -y `grep Build-Depends ldview/QT/debian/control | cut -d: -f2| sed 's/(.*)//g' | tr -d ,` libtinyxml-dev libgl2ps-dev
elif [ -f /etc/mandriva-release ] ; then
true

elif grep -q -e openSUSE /etc/os-release ; then
	zypper --non-interactive install git rpm-build rpmlint
	gitdownload
	zypper --non-interactive install `rpmbuild --nobuild ldview/QT/LDView.spec 2>&1  | grep 'needed by'| awk ' {print $1}'`
	zypper --non-interactive install `rpmbuild --nobuild ldview/QT/LDView-qt5.spec 2>&1  | grep 'needed by'| awk ' {print $1}'`
true
fi


