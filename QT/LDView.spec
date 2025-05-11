%define qt5 0
%define qt6 0

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

%if 0%{?fedora} || 0%{?centos_version}>=700 || 0%{?rhel_version}>=700 || 0%{?rhel} >=7 || 0%{?scientificlinux_version}>=700 || 0%{?suse_version}>=1300 || 0%{?mageia} || 0%{?oraclelinux}>=7 || 0%{?openeuler_version} || 0%{?almalinux} || 0%{?rocky_ver}
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
%if 0%{?qt6}
Name: ldview-qt6
%define without_osmesa 1
%else
Name: ldview
%endif
%endif
%if 0%{?suse_version} || 0%{?sles_version}
Group: Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?rhel}
Group: Graphics
%endif
%if 0%{?fedora} || 0%{?centos_version}
Group: Amusements/Graphics
%endif
Version: 4.6.1
%if 0%{?opensuse_bs}
Release: <CI_CNT>.<B_CNT>%{?dist}
%else
Release: 0.1%{?dist}
#Release: 0.0.beta2%{?dist}
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?rhel} || 0%{?fedora} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?mageia} || 0%{?oraclelinux} || 0%{?almalinux} || 0%{?rocky_ver}
License: GPLv2+
%else
%if 0%{?suse_version} || 0%{?sles_version}
License: GPL-2.0+
BuildRequires: fdupes
%else
License: GPLv2+
%endif
%endif
URL: http://github.com/tcobbs/ldview
Vendor: Travis Cobbs <ldview@gmail.com>
%if 0%{?opensuse_bs}!=1
Packager: Peter Bartfai <pbartfai@stardust.hu>
%endif
BuildRoot: %{_builddir}/%{name}
#Requires: unzip

%if 0%{?fedora} || 0%{?rhel_version} || 0%{?rhel} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?oraclelinux} || 0%{?openeuler_version} || 0%{?almalinux} || 0%{?rocky_ver}
%if 0%{?fedora} || 0%{?oraclelinux} > 6 || 0%{?centos_version} || 0%{?rhel_version} || 0%{?rhel} || 0%{?rocky_ver}
BuildRequires: hostname, which
%endif
%if ( 0%{?centos_version}>=600 || 0%{?rhel_version}>=600 || 0%{?rhel} > 6 || 0%{?scientificlinux_version}>=600 || 0%{?oraclelinux}>=6 || 0%{?openeuler_version} || 0%{?almalinux} || 0%{?rocky_ver})
BuildRequires: minizip-compat-devel
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
%if 0%{?qt6}
BuildRequires: qt6-qtbase-devel, qt6-linguist, qt6-qt5compat-devel
%else
BuildRequires: qt-devel
%endif
%endif
%endif
#BuildRequires: boost-devel
%if 0%{?opensuse_bs}!=1
BuildRequires: git
%endif
%if (0%{?rhel_version} || 0%{?rhel} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?oraclelinux} || 0%{?openeuler_version} || 0%{?almalinux} || 0%{?rocky_ver})
%if 0%{?rhel_version} == 700
BuildRequires: kdelibs-devel
%else
%if 0%{?centos_version} < 800 && 0%{?rhel_version} < 800 && 0%{?rhel} < 8 && 0%{?oraclelinux} < 7 && 0%{?almalinux_ver} == 0 && 0%{?rocky_ver} < 7
BuildRequires: kdebase-devel
%endif
%endif
BuildRequires: libjpeg-turbo-devel
%else
BuildRequires: libjpeg-turbo-devel, kf5-kio-devel, extra-cmake-modules, kf5-kdelibs4support
%endif
BuildRequires: gcc-c++, libpng-devel, make
%endif

%if 0%{?fedora} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?rhel} || 0%{?scientificlinux_version} || 0%{?oraclelinux} || 0%{?openeuler_version} || 0%{?almalinux} || 0%{?rocky_ver}
%if 0%{?centos_version} < 800 && 0%{?oraclelinux} < 7 && 0%{?rhel_version} == 0 && 0%{?rhel} == 0
BuildRequires: mesa-libOSMesa-devel
%endif
BuildRequires: mesa-libGLU-devel
%endif

%if 0%{?rhel_version} || 0%{?rhel} || 0%{?oraclelinux}
%define without_osmesa 1
%define tinyxml_static 1
%define gl2ps_static   1
%endif
#Source0: LDView.tar.gz

%if 0%{?fedora}
BuildRequires: libjpeg-turbo-devel, tinyxml-devel, gl2ps-devel, minizip-compat-devel
%endif

%if 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?oraclelinux}
%define tinyxml_static 1
%define gl2ps_static   1
%endif

%if 0%{?suse_version}
BuildRequires: cmake, update-desktop-files, glu-devel, Mesa-devel, tinyxml-devel, hostname, minizip-devel
%if 0%{?is_opensuse}
BuildRequires: extra-cmake-modules, kio-devel, kdelibs4support
%endif
%if 0%{?qt5}
BuildRequires: libqt5-qtbase-devel, libqt5-linguist, zlib-devel, libpng16-compat-devel, libjpeg8-devel
%else
BuildRequires: libqt4-devel
%endif
%if 0%{?is_opensuse}
BuildRequires: gl2ps-devel
%endif
%if 0%{?opensuse_bs}
BuildRequires:	-post-build-checks
%endif
%endif

%if 0%{?sles_version}
%define tinyxml_static 1
%if 0%{?opensuse_bs}
BuildRequires:	-post-build-checks
%endif
BuildRequires: hostname
Requires(post): desktop-file-utils
%endif

%if 0%{?mageia}
BuildRequires: extra-cmake-modules, cmake, kdelibs4support, which
%ifarch x86_64
BuildRequires: lib64kf5kio-devel, lib64gl2ps-devel, lib64tinyxml-devel, lib64minizip-devel
%else
BuildRequires: libkf5kio-devel, libgl2ps-devel, libtinyxml-devel, libminizip-devel
%endif
%if 0%{?qt5}
BuildRequires: qttools5
%ifarch x86_64
BuildRequires: lib64qt5base5-devel, lib64mesaglu1-devel, lib64jpeg-devel
%else
BuildRequires: libqt5base5-devel, libmesaglu1-devel, libjpeg-devel
%endif
%else
#BuildRequires: kdelibs4-devel
%ifarch x86_64
BuildRequires: lib64osmesa-devel, lib64qt4-devel
%else
BuildRequires: libosmesa-devel, libqt4-devel
%endif
%endif
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
%if 0%{?almalinux_ver}
echo "AlmaLinux:          %{almalinux_ver}"
%endif
%if 0%{?oraclelinux}
echo "OracleLinux:        0%{?oraclelinux}"
%endif
%if 0%{?rocky_ver}
echo "RockyLinux:         %{rocky_ver}"
%endif
%if 0%{?fedora}
echo "Fedora:             %{fedora}"
%endif
%if 0%{?openeuler_version}
echo "openEuler:          %{openeuler_version}"
%endif
%if 0%{?rhel_version}
echo "RedHat:             %{rhel_version}"
%endif
%if 0%{?rhel}
echo "RedHat:             0%{?rhel}"
%endif
%if 0%{?scientificlinux_version}
echo "Scientific Linux:   %{scientificlinux_version}"
%endif
%if 0%{?mageia}
echo "Mageia:             %{mageia}"
%endif
set -x

%build
%define is_kde5 %(which kf5-config >/dev/null 2>/dev/null && echo 1 || echo 0)
%if 0%{?is_kde5} == 0
%define is_kde4 %(which kde4-config >/dev/null 2>/dev/null && echo 1 || echo 0)
%endif
cd $RPM_SOURCE_DIR/[Ll][Dd][Vv]iew/QT
%ifarch i386 i486 i586 i686
%define qplatform linux-g++-32
%else
%ifarch x86_64
%define qplatform linux-g++-64
%else
%define qplatform linux-g++
%endif
%endif
%if ( 0%{?centos_version}<600 && 0%{?centos_version}>=500 ) || ( 0%{?rhel_version}<600 && 0%{?rhel_version}>=500 )
if [ -x %{_libdir}/qt4/bin/qmake ] ; then
export PATH=%{_libdir}/qt4/bin:$PATH
fi
%endif
%if (0%{?qt5}!=1) && (0%{?qt6}!=1)
%ifarch x86_64
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -I%{_libdir}/qt4/include"
%endif
%endif
%if 0%{?qt5}
if which lrelease-qt5 >/dev/null 2>/dev/null ; then
        lrelease-qt5 LDView.pro
else
        lrelease LDView.pro
fi
%else
%if 0%{?qt6}
if which lrelease-pro >/dev/null 2>/dev/null ; then
	lrelease-pro LDView.pro
else
	lrelease-qt6 LDView.pro
fi
%else
if which lrelease-qt4 >/dev/null 2>/dev/null ; then
	lrelease-qt4 LDView.pro
else
	lrelease LDView.pro
fi
%endif
%endif
%if 0%{?qt5}
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -fPIC"
if which qmake-qt5 >/dev/null 2>/dev/null ; then
        qmake-qt5 -spec %{qplatform} %{use_cpp11}
else
        qmake -spec %{qplatform} %{use_cpp11}
fi
%else
%if 0%{?qt6}
if which qmake6 >/dev/null 2>/dev/null ; then
	qmake6 -spec %{qplatform} %{use_cpp11}
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
%endif
make TESTING="$RPM_OPT_FLAGS"
strip LDView
%if (0%{?qt5} != 1) && (0%{?qt6} != 1)
%if "%{without_osmesa}" != "1"
cd ../OSMesa
%if 0%{?cpp11}
export RPM_OPT_FLAGS="$RPM_OPT_FLAGS -DUSE_CPP11"
%endif
make clean
make TESTING="$RPM_OPT_FLAGS"
%endif
%endif
%if %{is_kde5}
cd ../QT/kde5
if [ -d build ]; then rm -rf build ; fi
mkdir -p build
cd build
if cmake -DCMAKE_C_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_CXX_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_INSTALL_PREFIX=`kf5-config --prefix` .. ; then
make
fi
%else
%if %{is_kde4}
cd ../QT/kde
if [ -d build ]; then rm -rf build ; fi
mkdir -p build
cd build
if cmake -DCMAKE_C_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_CXX_FLAGS_RELEASE="%{optflags}" \
-DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` .. ; then
make
fi
%endif
%endif

%install
cd $RPM_SOURCE_DIR/[Ll][Dd][Vv]iew/QT
make INSTALL_ROOT=$RPM_BUILD_ROOT install
strip $RPM_BUILD_ROOT%{_bindir}/LDView
%if "%{without_osmesa}" != "1"
strip ../OSMesa/ldview
install -m 755 ../OSMesa/ldview $RPM_BUILD_ROOT%{_bindir}/ldview
install -m 644 ../OSMesa/ldviewrc.sample \
		$RPM_BUILD_ROOT%{_datadir}/ldview/ldviewrc.sample
install -m 644 ../OSMesa/ldview.1 \
		$RPM_BUILD_ROOT%{_mandir}/man1/ldview.1
gzip -f $RPM_BUILD_ROOT%{_mandir}/man1/ldview.1
%endif
%if %{is_kde5}
if [ -f kde5/build/ldviewthumbnail.so ] ; then
	mkdir -p $RPM_BUILD_ROOT/%{_libdir}/qt5/plugins
	install -m 644 kde5/build/ldviewthumbnail.so \
		$RPM_BUILD_ROOT/%{_libdir}/qt5/plugins/ldviewthumbnail.so
	strip $RPM_BUILD_ROOT/%{_libdir}/qt5/plugins/ldviewthumbnail.so
fi
%else
%if %{is_kde4}
if [ -f kde/build/lib/ldviewthumbnail.so ] ; then
	mkdir -p $RPM_BUILD_ROOT/%{_libdir}/kde4
	install -m 644 kde/build/lib/ldviewthumbnail.so \
			$RPM_BUILD_ROOT/%{_libdir}/kde4/ldviewthumbnail.so
	strip $RPM_BUILD_ROOT/%{_libdir}/kde4/ldviewthumbnail.so
fi
%endif
%endif
%if 0%{?suse_version}
%suse_update_desktop_file ldview Graphics
%endif
%if 0%{?suse_version} || 0%{?sles_version}
%fdupes %buildroot/%{_datadir}
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
%if %{is_kde5}
%dir %{_libdir}/qt5/plugins
%{_libdir}/qt5/plugins/ldviewthumbnail.so
%dir %{_datadir}/kservices5
%{_datadir}/kservices5/ldviewthumbnailcreator.desktop
%else
%if %{is_kde4}
%dir %{_libdir}/kde4
%{_libdir}/kde4/ldviewthumbnail.so
%dir %{_datadir}/kde4/services
%{_datadir}/kde4/services/ldviewthumbnailcreator.desktop
%endif
%endif
%dir %{_datadir}/icons/gnome
%dir %{_datadir}/icons/gnome/32x32
%dir %{_datadir}/icons/gnome/32x32/mimetypes
%dir %{_datadir}/mime-info
%dir %{_datadir}/application-registry
%dir %{_datadir}/thumbnailers
%{_datadir}/mime-info/ldraw.mime
%{_datadir}/mime/packages/ldraw.xml
%{_datadir}/mime-info/ldraw.keys
%{_datadir}/application-registry/ldview.applications
%{_datadir}/applications/ldview.desktop
%{_datadir}/metainfo/io.github.tcobbs.LDView.metainfo.xml
%{_datadir}/thumbnailers/ldview.thumbnailer
%{_bindir}/ldraw-thumbnailer
%{_datadir}/pixmaps/gnome-ldraw.png
%{_datadir}/pixmaps/ldview.png
%{_datadir}/icons/gnome/32x32/mimetypes/gnome-mime-application-x-ldraw.png
%{_datadir}/icons/gnome/32x32/mimetypes/gnome-mime-application-x-multipart-ldraw.png
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
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?rhel} || 0%{?fedora} || 0%{?centos_version}
NAUTILUS=`pidof nautilus`
if [ -n "$NAUTILUS" ] ; then kill -HUP $NAUTILUS ; fi 
%endif

%postun
%if 0%{?suse_version} >= 1140
%desktop_database_postun
%endif
update-mime-database /usr/share/mime >/dev/null || true
update-desktop-database || true

%if "%{without_osmesa}" != "1"
%if (0%{?qt5} !=1) && (0%{?qt6} != 1)
%package osmesa
Summary: OSMesa port of LDView for servers without X11
%if 0%{?suse_version} || 0%{?sles_version}
Group: Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?rhel}
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
* Sat May 10 2025 - pbartfai (at) stardust.hu 4.6-1
- Added support for using an LDraw Library Zip file to load parts instead of files in the LDraw directory.
- Added support for cylh and edgh primitives (not in POV export).
- Use https for library and unofficial part downloads (except for command line-only OS-Mesa-based ldview).
- Deal more gracefully with the http 429 (Too many requests) status code. Specifically, don't record a failed download attempt when this code is received, and show an error to the user instructing them to wait a short time and then try reloading the file.
- Updated so that random colors produce the same colors each time you load a given model. (That was the original intended behavior. This feature is designed for parts authors, so having new colors show up every time they reload can be distracting.)
- Qt & Windows: Move the LDraw and Extra dirs functionality from the File menu to a new LDraw tab in Preferences.
- Fixed initial directory creation during unzip of LDraw Parts library.
- Fixed crash when canceling parts library update at the wrong time.
- Fix CYLINDRICAL TEXMAP to be right side up.
- Calculate best guess texture coords for on-axis vertices with CYLINDRICAL TEXMAPs.
- Fixed problem that resulted in duplicate and/or out of place conditional lines.
- Fixed problems when edge lines were enabled in POV exports.
- Use alpha channel for auto-crop when transparent background is enabled.
- Fixed highlighting of line geometry in the Model Tree.
- Fixed minor problems with !LDRAW_ORG meta processing.
- Mac: Fixed opening a file in LDView from Finder.

* Sun Apr 23 2023 - pbartfai (at) stardust.hu 4.5-1
- Added support for custom config LDraw file.
- Optimize checking for library updates to finish quickly when the latest available LDraw library update is detected in the local LDraw library.
- Mac: Now requires macOS 10.13 (High Sierra) or later.
- Mac: Improve toolbar buttons in Big Sur and later.
- Fixed unofficial part downloads from the updated ldraw.org server.
- Fixed LDraw library updates to work with new script on library.ldraw.org.
- Disabled BFC in opaque portions of transparent parts.
- Fixed green/red/blue front/back/neutral BFC faces to work when lighting is disabled.
- Fixed primitive substitution for 48/ cone primitives with 2-digit sizes.
- Fixed specular and shininess in parent model leaking into child models.
- Fixed possible threading problem with conditional lines.
- Fixed possible problem loading preferences.
- Fixed crash when textures were applied to colored geometry.
- Fixed problem where a primitive in a user model would prevent subsequent parts in the same model from being detected as being parts.
- Fixed directly opening a file from LDraw library to recognize the attributes (part, primitive, unofficial) of the directory from which it was loaded.
- Fixed memory leak in Model Tree
- Fixed model tree to treat LDraw files as UTF-8.
- Mac: Fixed possible problem with network address lookup.
- Mac: Fixed Errors & Warnings dialog to properly refresh when the model is reloaded after changing the Extra Search Folders.
- Mac: Fixed crash when creating a new preference set on an Apple Silicon Mac with macOS Monterey.
- Window: Fixed Model Tree search to pick up from the current selection when Highlight selected line is unchecked.

* Thu Feb 10 2022 - pbartfai (at) stardust.hu 4.4.1-1
- Added primitive substitution support for "tndis" (truncated not-disc) primitives.
- Fixed -CommandLinesList option ignoring some options on most lines.

* Mon Jun 15 2020 - pbartfai (at) stardust.hu 4.4-1
- Added support for SPHERICAL and CYLINDRICAL texture maps.
- Added support for !DATA meta-command.
- Updated formatting of lines from LDraw files in Errors & Warnings dialog to use double spaces around vertices.
- Added support for -SaveSnapshotsList=<path>, -ExportFilesList=<path>, and -CommandLinesList<path> command line options.
- Added ability to reset times for unofficial part downloads, to force LDView to check the Parts Tracker again when it next opens any of those files. Also, automatically do this when a new LDraw directory is chosen via the UI.
- Added ability to create Z map files (with ldvz extension) alongside snapshots.
- Added setting to enable or disable the background in POV exports.
- Added SnapshotTNMetas option to cause snapshot saves to write Gnome-compatible metadata to PNG snapshots.
- Windows: Added support for DPI scaling (AKA High DPI). Requires Windows 10 Version 1703 (AKA Creators Update) or later.
- Windows: Added "LDView.com", a wrapper for LDView64.exe and LDView.exe that is designed to be used from the command line so that errors and warnings can be displayed.
- Mac: Added Retina support.
- Mac: Added support for QuickLook thumbnails for .ldr and .mpd files. This can be disabled by unchecking the "Use LDView to generate thumbnails for LDraw files" check box in the LDraw tab of LDView's Preferences.
- Mac: Added LDraw file line content to Errors & Warnings panel.
- Mac: Added support for "Dark Mode".
- Mac: Added support for rotate (twist) trackpad gestures.
- Antialiased edge lines are now more visible visible around the edges of transparent geometry, although this has a slight performance penalty.
- Updated so that hitting space while the model is spinning will stop the rotation.
- Update default verbosity for command line snapshots and exports to show errors (but not warnings).
- Wrapped LDX... declarations in #ifndef statements to allow them to be declared in a wrapper file that includes LDView's generated POV file.
- Removed "Transparent textures last" option in the Primitives tab of preferences. It caused problems when it was unchecked, so now LDView always behaves like it used to behave when this was checked.
- Updated default POV file version to 3.7.
- Updated to use srgb colors in POV 3.7 (or later).
- Mac: Now requires macOS 10.8 Mountain Lion or later.
- Mac: No longer uses drawers (which are deprecated).
- Fixed texture coordinate memory corruption problem.
- Fixed primitive substitution of ring primitives with 2-digit sizes.
- Fixed crashes when quads or triangles had to be converted to something else and their coordinates had too many decimal places.
- Fixed problem where textures with alpha blending would produce invalid snapshot output when Transparent Background was enabled.
- Fixed problem where texmaps wouldn't be visible on studs when stud textures were enabled; this introduces a lesser problem, where no stud texture appears on studs with opaque texmaps on top, and parts of the stud texture show through if the texmap is partially transparent.
- Fixed problem where edge lines were hidden by texmaps.
- Fixed problem where using texmaps on geometry that is not color 16 would cause corruption.
- Fixed texmaps not working properly on triangle fans.
- Fixed texmaps to work with BFC INVERTNEXT geometry.
- Fixed to support loading LDraw files with arbitrarily long lines.
- Mac: Fixed command line snapshot generation to work in macOS High Sierra and later.
- Mac: Fixed Errors & Warnings panel "Copy Error to Clipboard" button.
- Mac: Fixed Errors & Warnings panel to not truncate rows.
- Mac: Fixed Model Tree panel to not truncate rows.
- Mac: Fixed full screen support.
- Mac: Fixed behavior when macOS has been configured to treat the three-finger trackpad swipe gesture as a mouse drag gesture with the left button pressed.

* Tue Jan 30 2018 - pbartfai (at) stardust.hu 4.3-1
- Added support for command line exports.
- Added ability to specify an alternate LDConfig.ldr file on the command line.
- Added support for -IniFile=<path> on the command line.
- Added Keep Right Side Up feature to Mac version.
- Added support for mixed-resolution torus primitives.
- Added FreeBSD packaging support
- Added AppImage packaging support
- Added new texture mapping settings to allow user control over tradeoffs introduced with the texture mapping fix for transparent surfaces.
- Updated Mac, Qt, and OSMesa versions to optionally show errors and warnings when used to generate snapshots or perform exports from the command line.
- Updated "What's This?" help in Windows version to work on modern versions of Windows.
- Updated Linux packaging scripts for RPM and Debian based distros
- Fixed crash when loading a file that contained a spherical texmap.
- Fixed "Extra search directories" to work again.
- Fixed OSMesa version to work properly on case-sensitive file systems.
- Various fixes of memory leaks and potential (unlikely) crashes based on output from the Xcode static analyzer.
- Fixed Windows screen saver to use the LDraw directory set in the main application.
- Fixed POV-Ray export problem with 48/ torus primitives.
- In Windows, ignore the saved window position of any dialog when it results in a window that is less than half visible.
- Ignore UTF-8 Byte Order Mark (BOM) if it is present in a model file.
- Fixed so that texture mapping a transparent surface doesn't make that surface opaque.
- Fixed crash when a !TEXMAP meta-command in an LDraw file does not result in any textured geometry.

* Tue Sep 25 2012 - pbartfai (at) stardust.hu 4.2-1
- Changelog added
- Moved files from /usr/local to /usr
- General cleanup for rpmlint checkups
