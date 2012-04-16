Summary: 3D Viewer for LDraw models
Name: ldview
Group: Applications/Multimedia
Version: 4.2Beta
Release: 1%{?dist}
License: GPL
URL: http://ldview.sourceforge.net
Vendor: Travis Cobbs <ldview@gmail.com>
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRoot: %{_builddir}/%{name}
Requires: unzip
BuildRequires: qt-devel, boost-devel, cvs, kdebase-devel, mesa-libOSMesa-devel, gcc-c++, libpng-devel, make

%description
LDView is a real-time 3D viewer for displaying LDraw models using hardware-accellerated 3D graphics. It was written using OpenGL, so should be accellerated on any video card which provides full OpenGL 3D accelleration (so-called mini-drivers are not likely to work). It should also work on other video cards using OpenGL software rendering, albeit at a much slower speed. For information on LDraw, please visit www.ldraw.org, the centralized LDraw information site. 

The program can read LDraw DAT files as well as MPD files. It then allows you to rotate the model around to any angle with themouse. A fast computer or a video card with T&L support (Transform & Lighting) is strongly recommended for displaying complexmodels. 

%prep
cd $RPM_SOURCE_DIR
cvs -z3 -d:pserver:anonymous@ldview.cvs.sourceforge.net/cvsroot/ldview co LDView

%build
cd $RPM_SOURCE_DIR/LDView/QT
%ifarch i386 i486 i586 i686
%define qplatform linux-g++-32
%endif
%ifarch x86_64
%define qplatform linux-g++-64
%endif
if which qmake-qt4 >/dev/null 2>/dev/null ; then
	qmake-qt4 -spec %{qplatform}
else
	qmake -spec %{qplatform}
fi
make clean
make TESTING="%{optflags}"
if which lrelease-qt4 >/dev/null 2>/dev/null ; then
	lrelease-qt4 LDView.pro
else
	lrelease LDView.pro
fi
strip LDView
cd ../OSMesa
make clean
make TESTING="%{optflags}"
cd ../QT/kde
if [ -d build ]; then rm -rf build ; fi
mkdir -p build
cd build
cmake -DCMAKE_C_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_CXX_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
make

%install
cd $RPM_SOURCE_DIR/LDView/QT
mkdir -p $RPM_BUILD_ROOT/usr/local/share/ldview
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
install -d $RPM_BUILD_ROOT/usr/local/share/ldview
install -m 755 LDView $RPM_BUILD_ROOT/usr/local/bin/LDView
install -m 755 ../OSMesa/ldview $RPM_BUILD_ROOT/usr/local/bin/ldview
install -m 644 ../Textures/SansSerif.fnt \
$RPM_BUILD_ROOT/usr/local/share/ldview/SansSerif.fnt
install -m 644 ../Help.html $RPM_BUILD_ROOT/usr/local/share/ldview/Help.html
install -m 644 ../Readme.txt $RPM_BUILD_ROOT/usr/local/share/ldview/Readme.txt
install -m 644 ../ChangeHistory.html \
				$RPM_BUILD_ROOT/usr/local/share/ldview/ChangeHistory.html
install -m 644 ../license.txt \
				$RPM_BUILD_ROOT/usr/local/share/ldview/license.txt
install -m 644 ../m6459.ldr $RPM_BUILD_ROOT/usr/local/share/ldview/m6459.ldr
install -m 644 ../8464.mpd $RPM_BUILD_ROOT/usr/local/share/ldview/8464.mpd 
install -m 644 ../LDViewMessages.ini \
				$RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages.ini
cat ../LDExporter/LDExportMessages.ini >> \
				$RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages.ini
install -m 644 ../Translations/German/LDViewMessages.ini \
				$RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages_de.ini
install -m 644 ../Translations/Italian/LDViewMessages.ini \
				$RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages_it.ini
install -m 644 ../Translations/Czech/LDViewMessages.ini \
			    $RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages_cz.ini
install -m 644 ../Translations/Hungarian/LDViewMessages.ini \
				$RPM_BUILD_ROOT/usr/local/share/ldview/LDViewMessages_hu.ini
install -m 644 todo.txt $RPM_BUILD_ROOT/usr/local/share/ldview/todo.txt
install -m 644 ldview_en.qm $RPM_BUILD_ROOT/usr/local/share/ldview/ldview_en.qm
install -m 644 ldview_de.qm $RPM_BUILD_ROOT/usr/local/share/ldview/ldview_de.qm
install -m 644 ldview_it.qm $RPM_BUILD_ROOT/usr/local/share/ldview/ldview_it.qm
install -m 644 ldview_cz.qm $RPM_BUILD_ROOT/usr/local/share/ldview/ldview_cz.qm
install -m 644 ../LDExporter/LGEO.xml \
			   $RPM_BUILD_ROOT/usr/local/share/ldview/LGEO.xml
mkdir -p $RPM_BUILD_ROOT/usr/share/mime-info/
mkdir -p $RPM_BUILD_ROOT/usr/share/mime/packages/
mkdir -p $RPM_BUILD_ROOT/usr/share/application-registry/
mkdir -p $RPM_BUILD_ROOT/usr/share/applications/
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/pixmaps/
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes
mkdir -p $RPM_BUILD_ROOT/etc/gconf/schemas
install -m 644 desktop/ldraw.mime $RPM_BUILD_ROOT/usr/share/mime-info/ldraw.mime
install -m 644 desktop/ldraw.xml  \
				$RPM_BUILD_ROOT/usr/share/mime/packages/ldraw.xml
install -m 644 desktop/ldraw.keys $RPM_BUILD_ROOT/usr/share/mime-info/ldraw.keys
install -m 644 desktop/ldview.applications \
			$RPM_BUILD_ROOT/usr/share/application-registry/ldview.applications
install -m 644 desktop/ldraw.desktop \
				$RPM_BUILD_ROOT/usr/share/applications/ldraw.desktop
install -m 755 desktop/ldraw-thumbnailer \
				$RPM_BUILD_ROOT/usr/bin/ldraw-thumbnailer
install -m 644 images/LDViewIcon.png \
				$RPM_BUILD_ROOT/usr/share/pixmaps/gnome-ldraw.png
install -m 644 images/LDViewIcon.png $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
install -m 644 images/LDViewIcon.png $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png
install -m 644 desktop/ldraw.schemas \
			$RPM_BUILD_ROOT/etc/gconf/schemas/ldraw.schemas
mkdir -p $RPM_BUILD_ROOT/usr/share/kde4/services
install -m 644 kde/ldviewthumbnailcreator.desktop \
		$RPM_BUILD_ROOT/usr/share/kde4/services/ldviewthumbnailcreator.desktop
%ifarch x86_64
mkdir -p $RPM_BUILD_ROOT/usr/lib64/kde4
install -m 644 kde/build/lib/ldviewthumbnail.so \
				$RPM_BUILD_ROOT/usr/lib64/kde4/ldviewthumbnail.so
%else
mkdir -p $RPM_BUILD_ROOT/usr/lib/kde4
install -m 644 kde/build/lib/ldviewthumbnail.so \
				$RPM_BUILD_ROOT/usr/lib/kde4/ldviewthumbnail.so
%endif

%files
/usr/local/bin/LDView
/usr/local/share/ldview/SansSerif.fnt
/usr/local/share/ldview/Help.html
/usr/local/share/ldview/license.txt
/usr/local/share/ldview/ChangeHistory.html
/usr/local/share/ldview/m6459.ldr
/usr/local/share/ldview/8464.mpd
/usr/local/share/ldview/Readme.txt
/usr/local/share/ldview/LDViewMessages.ini
/usr/local/share/ldview/LDViewMessages_de.ini
/usr/local/share/ldview/LDViewMessages_it.ini
/usr/local/share/ldview/LDViewMessages_cz.ini
/usr/local/share/ldview/LDViewMessages_hu.ini
/usr/local/share/ldview/todo.txt
/usr/local/share/ldview/ldview_en.qm
/usr/local/share/ldview/ldview_de.qm
/usr/local/share/ldview/ldview_it.qm
/usr/local/share/ldview/ldview_cz.qm
/usr/local/share/ldview/LGEO.xml
%ifarch x86_64
/usr/lib64/kde4/ldviewthumbnail.so
%else
/usr/lib/kde4/ldviewthumbnail.so
%endif
/usr/share/kde4/services/ldviewthumbnailcreator.desktop
/usr/share/mime-info/ldraw.mime
/usr/share/mime/packages/ldraw.xml
/usr/share/mime-info/ldraw.keys
/usr/share/application-registry/ldview.applications
/usr/share/applications/ldraw.desktop
/usr/bin/ldraw-thumbnailer
/usr/share/pixmaps/gnome-ldraw.png
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png
/etc/gconf/schemas/ldraw.schemas

%clean
rm -rf $RPM_BUILD_ROOT

%post
update-mime-database  /usr/share/mime >/dev/null
update-desktop-database
cd /etc/gconf/schemas
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
gconftool-2 --makefile-install-rule ldraw.schemas >/dev/null
kill -HUP `pidof nautilus`

%postun
update-mime-database  /usr/share/mime >/dev/null
update-desktop-database

%preun
cd /etc/gconf/schemas
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
gconftool-2 --makefile-uninstall-rule ldraw.schemas >/dev/null

%package osmesa
Summary: OSMesa port of LDView for servers without X11
Group: Applications/Multimedia
%description osmesa
OSMesa port of LDView for servers without X11

%files osmesa
/usr/local/bin/ldview

