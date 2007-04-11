Summary: 3D Viewer for LDraw models
Name: ldview
Group: Applications/Multimedia
Version: 3.1
Release: 1
License: GPL
URL: http://ldview.sourceforge.net
Vendor: Travis Cobbs <ldview@gmail.com>
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRoot: %{_builddir}/%{name}
BuildPreReq: qt-devel, boost-devel, cvs

%description
LDView is a real-time 3D viewer for displaying LDraw models using hardware-accellerated 3D graphics. It was written using OpenGL, so should be accellerated on any video card which provides full OpenGL 3D accelleration (so-called mini-drivers are not likely to work). It should also work on other video cards using OpenGL software rendering, albeit at a much slower speed. For information on LDraw, please visit www.ldraw.org, the centralized LDraw information site. 

The program can read LDraw DAT files as well as MPD files. It then allows you to rotate the model around to any angle with themouse. A fast computer or a video card with T&L support (Transform & Lighting) is strongly recommended for displaying complexmodels. 

%prep
cd $RPM_SOURCE_DIR
cvs -z3 -d:pserver:anonymous@ldview.cvs.sourceforge.net/cvsroot/ldview co LDView

%build
cd $RPM_SOURCE_DIR/LDView/QT
qmake
make
strip LDView

%install
cd $RPM_SOURCE_DIR/LDView/QT
mkdir -p $RPM_BUILD_ROOT/usr/local/share/ldview
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
install -d $RPM_BUILD_ROOT/usr/local/share/ldview
install -m 755 LDView $RPM_BUILD_ROOT/usr/local/bin/LDView
install -m 644 ../Textures/SansSerif.fnt \
$RPM_BUILD_ROOT/usr/local/share/ldview/SansSerif.fnt
install -m 644 ../Help.html $RPM_BUILD_ROOT/usr/local/share/ldview/Help.html
install -m 644 ../Readme.txt $RPM_BUILD_ROOT/usr/local/share/ldview/Readme.txt
install -m 644 ../ChangeHistory.html $RPM_BUILD_ROOT/usr/local/share/ldview/ChangeHistory.html
install -m 644 ../license.txt $RPM_BUILD_ROOT/usr/local/share/ldview/license.txt
install -m 644 ../m6459.ldr $RPM_BUILD_ROOT/usr/local/share/ldview/m6459.ldr
install -m 644 ../8464.mpd $RPM_BUILD_ROOT/usr/local/share/ldview/8464.mpd 
install -m 644 ../LDViewMessages.ini \
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
mkdir -p $RPM_BUILD_ROOT/usr/share/mime-info/
mkdir -p $RPM_BUILD_ROOT/usr/share/mime/packages/
mkdir -p $RPM_BUILD_ROOT/usr/share/application-registry/
mkdir -p $RPM_BUILD_ROOT/usr/share/applications/
mkdir -p $RPM_BUILD_ROOT/usr/bin
mkdir -p $RPM_BUILD_ROOT/usr/share/pixmaps/gnome-ldraw.png
mkdir -p $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes
mkdir -p $RPM_BUILD_ROOT/etc/gconf/schemas
install -m 644 desktop/ldraw.mime $RPM_BUILD_ROOT/usr/share/mime-info/ldraw.mime
install -m 644 desktop/ldraw.xml  $RPM_BUILD_ROOT/usr/share/mime/packages/ldraw.xml
install -m 644 desktop/ldraw.keys $RPM_BUILD_ROOT/usr/share/mime-info/ldraw.keys
install -m 644 desktop/ldview.applications $RPM_BUILD_ROOT/usr/share/application-registry/ldview.applications
install -m 644 desktop/ldraw.desktop $RPM_BUILD_ROOT/usr/share/applications/ldraw.desktop
install -m 755 desktop/ldraw-thumbnailer $RPM_BUILD_ROOT/usr/bin/ldraw-thumbnailer
install -m 644 images/LDViewIcon.png $RPM_BUILD_ROOT/usr/share/pixmaps/gnome-ldraw.png
install -m 644 images/LDViewIcon.png $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
install -m 644 images/LDViewIcon.png $RPM_BUILD_ROOT/usr/share/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png
install -m 644 desktop/ldraw.schemas $RPM_BUILD_ROOT/etc/gconf/schemas/ldraw.schemas

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

%clean
rm -rf $RPM_BUILD_ROOT

%package gnome
Summary: Gnome integration for LDView
Group: Applications/Multimedia
PreReq: GConf2, shared-mime-info, desktop-file-utils
%description gnome
Gnome integration for LDView

%files gnome
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

%post gnome
update-mime-database  /usr/share/mime >/dev/null
update-desktop-database
cd /etc/gconf/schemas
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
gconftool-2 --makefile-install-rule ldraw.schemas >/dev/null
kill -HUP `pidof nautilus`

%postun gnome
update-mime-database  /usr/share/mime >/dev/null
update-desktop-database

%preun gnome
cd /etc/gconf/schemas
GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source` \
gconftool-2 --makefile-uninstall-rule ldraw.schemas >/dev/null

