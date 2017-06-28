%define qt5 0

%if 0%{?suse_version}
%define dist .openSUSE%(echo %{suse_version} | sed 's/0$//')
%endif

%if 0%{?sles_version}
%define dist .SUSE%(echo %{sles_version} | sed 's/0$//')
%endif

%if %(if [[ "%{vendor}" == obs://* ]] ; then echo 1 ; else echo 0 ; fi)
%define opensuse_bs 1
%endif

%if 0%{?centos_ver}
%define centos_version %{centos_ver}00
%endif

%if 0%{?fedora} || 0%{?centos_version}>=700 || 0%{?rhel_version}>=700 || 0%{?scientificlinux_version}>=700 || 0%{?suse_version}>=1300 || 0%{?mageia}
%define use_cpp11 USE_CPP11=YES
%define cpp11 1
%else
%define use_cpp11 USE_CPP11=NO
BuildRequires: boost-devel
%endif

Summary: 3D Viewer for LDraw models
%if 0%{?qt5}
Name: ldview-qt5
%define without_osmesa 1
%else
Name: ldview
%endif
%if 0%{?suse_version} || 0%{?sles_version}
Group: Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} 
Group: Graphics
%endif
%if 0%{?fedora} || 0%{?centos_version}
Group: Amusements/Graphics
%endif
Version: 4.3
%if 0%{?opensuse_bs}
Release: Beta2.<CI_CNT>.<B_CNT>%{?dist}
%else
Release: Beta2.1%{?dist}
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?fedora} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?mageia}
License: GPLv2+
%endif
%if 0%{?suse_version} || 0%{?sles_version}
License: GPL-2.0+
BuildRequires: fdupes
%endif
URL: http://github.com/tcobbs/ldview
Vendor: Travis Cobbs <ldview@gmail.com>
%if 0%{?opensuse_bs}!=1
Packager: Peter Bartfai <pbartfai@stardust.hu>
%endif
BuildRoot: %{_builddir}/%{name}
Requires: unzip

%if 0%{?fedora} || 0%{?rhel_version} || 0%{?centos_version} || 0%{?scientificlinux_version}
%if ( 0%{?centos_version}>=600 || 0%{?rhel_version}>=600 || 0%{?scientificlinux_version}>=600 )
%if 0%{?qt5}
# Qt5 Not supported
BuildRequires: qt5-qtbase-devel, qt5-linguist
%else
BuildRequires: qt-devel
%endif
%else
%if 0%{?qt5}
# Qt5 Not supported
BuildRequires: qt5-qtbase-devel
%endif
%endif
%if 0%{?fedora}
%if 0%{?qt5}
BuildRequires: qt5-qtbase-devel, qt5-linguist
%else
BuildRequires: qt-devel
%endif
%endif
#BuildRequires: boost-devel
%if 0%{?opensuse_bs}!=1
BuildRequires: git
%endif
%if (0%{?rhel_version}<700 && 0%{?centos_version}<700 && 0%{?scientificlinux_version}<600 && 0%{?fedora}<26)
BuildRequires: kdebase-devel
%else
BuildRequires: libjpeg-turbo-devel, kdelibs-devel
%endif
BuildRequires: gcc-c++, libpng-devel, make
%endif

%if 0%{?fedora} || 0%{?centos_version} || 0%{?scientificlinux_version}
BuildRequires: mesa-libOSMesa-devel
%endif

%if 0%{?rhel_version}
%define without_osmesa 1
%define tinyxml_static 1
%define gl2ps_static   1
%endif
#Source0: LDView.tar.gz

%if 0%{?fedora}
BuildRequires: libjpeg-turbo-devel, tinyxml-devel, gl2ps-devel
%if 0%{?opensuse_bs}
BuildRequires: samba4-libs
%if 0%{?fedora_version}==22
BuildRequires: qca
%endif
%if 0%{?fedora_version}==23
BuildRequires: qca, gnu-free-sans-fonts
%endif
%if 0%{?fedora_version}==25
BuildRequires: llvm-libs
%endif
%if 0%{?fedora_version}==26
BuildRequires: openssl-devel, storaged
%endif
%endif
%endif

%if 0%{?centos_version} || 0%{?scientificlinux_version}
%define tinyxml_static 1
%define gl2ps_static   1
%endif

%if 0%{?suse_version}
%if 0%{?suse_version}!=1315
%kde4_runtime_requires
BuildRequires: libkde4-devel
%else
%if 0%{?qt5}
BuildRequires: libpng16-compat-devel, libjpeg8-devel, libqt5-linguist
%endif
%endif
BuildRequires: cmake, update-desktop-files
%if 0%{?qt5}
BuildRequires: libqt5-qtbase-devel, zlib-devel
%else
BuildRequires: libqt4-devel
%endif
Requires(pre): gconf2
%if (0%{?suse_version} > 1210 && 0%{?suse_version}!=1315)
BuildRequires: gl2ps-devel
%else
%define gl2ps_static   1
%endif
%if 0%{?suse_version} > 1220
BuildRequires: glu-devel
%endif
%if 0%{?suse_version} > 1300
BuildRequires: Mesa-devel
%endif
%define tinyxml_static 1
%if 0%{?opensuse_bs}
BuildRequires:	-post-build-checks
%endif
#BuildRequires: gconf2-devel
#%%if 0%{?suse_version} == 1110
#%%gconf_schemas_prereq
#%%else 
#%%gconf_schemas_requires
#%%endif
%endif

%if 0%{?sles_version}
# SLE 11 SP3 has no libOSMesa.so
%define osmesa_found %(test -f /usr/lib/libOSMesa.so -o -f /usr/lib64/libOSMesa.so && echo 1 || echo 0)
%if "%{osmesa_found}" != "1"
%define without_osmesa 1
%endif
%define tinyxml_static 1
%if 0%{?opensuse_bs}
BuildRequires:	-post-build-checks
%endif
Requires(post): desktop-file-utils
%endif

%if 0%{?mageia}
%if 0%{?qt5}
BuildRequires: qttools5
%ifarch x86_64
BuildRequires: lib64qt5base5-devel, lib64mesaglu1-devel, lib64jpeg-devel
%else
BuildRequires: libqt5base5-devel, libmesaglu1-devel, libjpeg-devel
%endif
%if 0%{?opensuse_bs}
%ifarch x86_64
BuildRequires: lib64sane1, lib64proxy-webkit
%else
BuildRequires: libsane1, libproxy-webkit
%endif
%endif
%else
BuildRequires: boost-devel, cmake, kdelibs4-devel
%ifarch x86_64
BuildRequires: lib64osmesa-devel, lib64qt4-devel
%else
BuildRequires: libosmesa-devel, libqt4-devel
%endif
%if 0%{?opensuse_bs}
BuildRequires: phonon-vlc, gnome-shell, wget
%ifarch x86_64
BuildRequires: lib64sane1, lib64apr1_0, lib64apr-util1_0, lib64proxy-webkit
%else
BuildRequires: libsane1, libproxy-webkit
%endif
%endif
%endif
%endif

%if 0%{?mdkversion}
BuildRequires: libqt4-devel, boost-devel, cmake, kdelibs4-devel
%define gl2ps_static   1
# For openSUSE Build Service
%if 0%{?opensuse_bs}
%if (0%{?mdkversion} != 200910) && (0%{?mdkversion} != 201000)
BuildRequires: kde-l10n-en_GB
%endif
BuildRequires: aspell-en, myspell-en_US
%endif
%define tinyxml_static 1
%define without_osmesa 1
%endif

%if ( 0%{?centos_version}<600 && 0%{?centos_version}>=500 ) || ( 0%{?rhel_version}<600 && 0%{?rhel_version}>=500 )
BuildRequires: qt4-devel
%endif

%description
LDView is a real-time 3D viewer for displaying LDraw models using
hardware-accelerated 3D graphics. LDView is capable of reading LDraw DAT,
LDR and MPD files as well as parse subassemblies for the latter,
using the multipart tools menu. You can view and rotate the model around
to any angle using the mouse or keyboard. LDView was written using OpenGL:
it should provide accelerated rendering on any video card capable of
full OpenGL 3D acceleration (so-called 'mini-drivers' are not likely to work).

It should also work on other video cards using OpenGL software rendering,
albeit at a much slower speed. A  fast computer or a dedicated video card
with T&L support (Transform & Lighting) is strongly recommended
for displaying complex models. For information on LDraw,
please visit www.ldraw.org, the centralized LDraw resources site.

%prep
cd $RPM_SOURCE_DIR
if [ -s LDView.tar.gz ] ; then
	if [ -d LDView ] ; then rm -rf LDView ; fi
	tar zxf LDView.tar.gz
else
	if [ -f ldview-*.tar.gz ] ; then
		if [ -d ldview ] ; then rm -rf ldview ; fi
		mkdir ldview
		cd ldview
		tar zxf ../ldview-*.tar.gz --strip=1
	elif [ -d LDView ] ; then
		cd LDView
		git pull
		cd ..
	else
		ls
		git clone https://github.com/tcobbs/ldview LDView
	fi
fi
set +x
echo "_bindir:            %{_bindir}"
echo "_datadir:           %{_datadir}"
echo "_libdir:            %{_libdir}"
echo "_mandir:            %{_mandir}"
echo "_sysconfdir:        %{_sysconfdir}"
%if 0%{?opensuse_bs}
echo "OBS:                detected"
%endif
echo "Vendor:             %{?vendor}"
echo "Packager:           %{?packager}"
%if 0%{?suse_version}
echo "SUSE:               %{suse_version}"
%endif
%if 0%{?sles_version}
echo "SLES:               %{sles_version}"
%endif
%if 0%{?centos_ver}
echo "CentOS:             %{centos_ver}"
%endif
%if 0%{?fedora}
echo "Fedora:             %{fedora}"
%endif
%if 0%{?rhel_version}
echo "RedHat:             %{rhel_version}"
%endif
%if 0%{?scientificlinux_version}
echo "Scientific Linux:   %{scientificlinux_version}"
%endif
%if 0%{?mageia}
echo "Mageia:             %{mageia}"
%endif
set -x

%build
%define is_kde4 %(which kde4-config >/dev/null && echo 1 || echo 0)
cd $RPM_SOURCE_DIR/[Ll][Dd][Vv]iew/QT
%ifarch i386 i486 i586 i686
%define qplatform linux-g++-32
%endif
%ifarch x86_64
%define qplatform linux-g++-64
%endif
%if ( 0%{?centos_version}<600 && 0%{?centos_version}>=500 ) || ( 0%{?rhel_version}<600 && 0%{?rhel_version}>=500 )
if [ -x %{_libdir}/qt4/bin/qmake ] ; then
export PATH=%{_libdir}/qt4/bin:$PATH
fi
%endif
%if (0%{?qt5}!=1)
%ifarch x86_64
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -I%{_libdir}/qt4/include"
%endif
%endif
%if 0%{?fedora}==23
%ifarch x86_64
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -fPIC"
export Q_CXXFLAGS="$Q_CXXFLAGS -fPIC"
%endif
%endif
%if 0%{?qt5}
if which lrelease-qt5 >/dev/null 2>/dev/null ; then
        lrelease-qt5 LDView.pro
else
        lrelease LDView.pro
fi
%else
if which lrelease-qt4 >/dev/null 2>/dev/null ; then
	lrelease-qt4 LDView.pro
else
	lrelease LDView.pro
fi
%endif
%if 0%{?qt5}
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -fPIC"
if which qmake-qt5 >/dev/null 2>/dev/null ; then
        qmake-qt5 -spec %{qplatform} %{use_cpp11}
else
        qmake -spec %{qplatform} %{use_cpp11}
fi
%else
if which qmake-qt4 >/dev/null 2>/dev/null ; then
	qmake-qt4 -spec %{qplatform} %{use_cpp11}
else
	qmake -spec %{qplatform} %{use_cpp11}
fi
%endif
make TESTING="$RPM_OPT_FLAGS"
strip LDView
%if 0%{?qt5} != 1
%if "%{without_osmesa}" != "1"
cd ../OSMesa
%if 0%{?cpp11}
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -std=c++11"
%endif
make clean
make TESTING="$RPM_OPT_FLAGS"
%endif
%endif
cd ../QT/kde
if [ -d build ]; then rm -rf build ; fi
mkdir -p build
cd build
if cmake -DCMAKE_C_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_CXX_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` .. ; then
make
fi

%install
cd $RPM_SOURCE_DIR/[Ll][Dd][Vv]iew/QT
make INSTALL_ROOT=$RPM_BUILD_ROOT install
%if "%{without_osmesa}" != "1"
strip ../OSMesa/ldview
install -m 755 ../OSMesa/ldview $RPM_BUILD_ROOT%{_bindir}/ldview
install -m 644 ../OSMesa/ldviewrc.sample \
		$RPM_BUILD_ROOT%{_datadir}/ldview/ldviewrc.sample
install -m 644 ../OSMesa/ldview.1 \
		$RPM_BUILD_ROOT%{_mandir}/man1/ldview.1
gzip -f $RPM_BUILD_ROOT%{_mandir}/man1/ldview.1
%endif
if [ -f kde/build/lib/ldviewthumbnail.so ] ; then
	mkdir -p $RPM_BUILD_ROOT/%{_libdir}/kde4
	install -m 644 kde/build/lib/ldviewthumbnail.so \
			$RPM_BUILD_ROOT/%{_libdir}/kde4/ldviewthumbnail.so
	strip $RPM_BUILD_ROOT/%{_libdir}/kde4/ldviewthumbnail.so
fi
%if 0%{?suse_version}
%suse_update_desktop_file ldview Graphics
%endif
%if 0%{?suse_version} || 0%{?sles_version}
%fdupes %buildroot/%{_datadir}
#%%find_gconf_schemas
#%%def_gconf_schemas LDView
#%%add_gconf_schemas ldraw
#%%end_gconf_schemas
%endif

%files
%if 0%{?sles_version} || 0%{?suse_version}
%defattr(-,root,root)
%endif
%{_bindir}/LDView
%dir %{_datadir}/ldview
%doc %{_datadir}/ldview/ChangeHistory.html
%doc %{_datadir}/ldview/Help.html
%doc %{_datadir}/ldview/todo.txt
%doc %{_datadir}/ldview/Readme.txt
%doc %{_datadir}/ldview/license.txt
%{_datadir}/ldview/ldview_*.qm
%{_datadir}/ldview/LDViewMessages*.ini
%{_datadir}/ldview/LGEO.xml
%{_datadir}/ldview/SansSerif.fnt
%{_datadir}/ldview/8464.mpd
%{_datadir}/ldview/m6459.ldr
%if %{is_kde4}
%dir %{_libdir}/kde4
%{_libdir}/kde4/ldviewthumbnail.so
%endif
%dir %{_datadir}/kde4/services
%dir %{_sysconfdir}/gconf/schemas
%dir %{_datadir}/icons/gnome
%dir %{_datadir}/icons/gnome/32x32
%dir %{_datadir}/icons/gnome/32x32/mimetypes
%dir %{_datadir}/mime-info
%dir %{_datadir}/application-registry
%dir %{_datadir}/thumbnailers
%{_datadir}/kde4/services/ldviewthumbnailcreator.desktop
%{_datadir}/mime-info/ldraw.mime
%{_datadir}/mime/packages/ldraw.xml
%{_datadir}/mime-info/ldraw.keys
%{_datadir}/application-registry/ldview.applications
%{_datadir}/applications/ldview.desktop
%{_datadir}/thumbnailers/ldview.thumbnailer
%{_bindir}/ldraw-thumbnailer
%{_datadir}/pixmaps/gnome-ldraw.png
%{_datadir}/pixmaps/ldview.png
%{_datadir}/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
%{_datadir}/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png
%config(noreplace) %{_sysconfdir}/gconf/schemas/ldraw.schemas
%if 0%{?mdkversion} || 0%{?mageia}
%{_mandir}/man1/ldraw-thumbnailer.1.xz
%{_mandir}/man1/LDView.1.xz
%else
%{_mandir}/man1/ldraw-thumbnailer.1.gz
%{_mandir}/man1/LDView.1.gz
%endif

%clean
rm -rf $RPM_BUILD_ROOT
cd $RPM_SOURCE_DIR/[Ll][Dd][Vv]iew/QT
if [ -f Makefile ] ; then make -s clean ; fi
cd ../OSMesa
make -s clean

%post
%if 0%{?suse_version} >= 1140
%desktop_database_post
%endif
update-mime-database  /usr/share/mime >/dev/null || true
update-desktop-database || true
export GCONF_CONFIG_SOURCE="$(gconftool-2 --get-default-source)"
gconftool-2 --makefile-install-rule /etc/gconf/schemas/ldraw.schemas >/dev/null || true
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?fedora} || 0%{?centos_version}
NAUTILUS=`pidof nautilus`
if [ -n "$NAUTILUS" ] ; then kill -HUP $NAUTILUS ; fi 
%endif

%postun
%if 0%{?suse_version} >= 1140
%desktop_database_postun
%endif
update-mime-database /usr/share/mime >/dev/null || true
update-desktop-database || true

%pre
if [ "$1" -gt 1 ] ; then
export GCONF_CONFIG_SOURCE="$(gconftool-2 --get-default-source)"
if [ -f /etc/gconf/schemas/ldraw.schemas ] ; then
gconftool-2 --makefile-uninstall-rule /etc/gconf/schemas/ldraw.schemas >/dev/null || true
fi
fi

%preun
if [ "$1" -eq 0 ] ; then
export GCONF_CONFIG_SOURCE="$(gconftool-2 --get-default-source)"
if [ -f /etc/gconf/schemas/ldraw.schemas ] ; then
gconftool-2 --makefile-uninstall-rule /etc/gconf/schemas/ldraw.schemas >/dev/null || true
fi
fi

%if "%{without_osmesa}" != "1"
%if 0%{?qt5} !=1
%package osmesa
Summary: OSMesa port of LDView for servers without X11
%if 0%{?suse_version} || 0%{?sles_version}
Group: Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} 
Group: Graphics
%endif
%if 0%{?fedora} || 0%{?centos_version}
Group: Amusements/Graphics
%endif
%description osmesa
OSMesa port of LDView for servers without X11
No hardware acceleration is used.

%files osmesa
%if 0%{?sles_version} || 0%{?suse_version}
%defattr(-,root,root)
%endif
%{_bindir}/ldview
%doc %{_datadir}/ldview/ldviewrc.sample
%if 0%{?mdkversion} || 0%{?mageia}
%{_mandir}/man1/ldview.1.xz
%else
%{_mandir}/man1/ldview.1.gz
%endif
%endif
%endif

%changelog
* Tue Sep 25 2012 - pbartfai (at) stardust.hu 4.2-1
- Changelog added
- Moved files from /usr/local to /usr
- General cleanup for rpmlint checkups
