Summary: 3D Viewer for LDraw models
Name: ldview
Group: Applications/Multimedia
Version: 3.0RC1
Release: 1
License: MIT
URL: http://ldview.sourceforge.net
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRoot: %{_builddir}/%{name}

%description
LDView is a real-time 3D viewer for displaying LDraw models using hardware-accellerated 3D graphics. It was written using OpenGL, so should be accellerated on any video card which provides full OpenGL 3D accelleration (so-called mini-drivers are not likely to work). It should also work on other video cards using OpenGL software rendering, albeit at a much slower speed. For information on LDraw, please visit www.ldraw.org, the centralized LDraw information site. 

The program can read LDraw DAT files as well as MPD files. It then allows you to rotate the model around to any angle with themouse. A fast computer or a video card with T&L support (Transform & Lighting) is strongly recommended for displaying complexmodels. 

%prep
cd $RPM_SOURCE_DIR
cvs -z3 -d:pserver:anonymous@cvs.sourceforge.net/cvsroot/ldview co LDView

%build
cd $RPM_SOURCE_DIR/LDView/QT
qmake
./makeall
strip LDView

%install
cd $RPM_SOURCE_DIR/LDView/QT
mkdir -p $RPM_BUILD_ROOT/usr/local/share/ldview
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
install -d $RPM_BUILD_ROOT/usr/local/share/ldview
install -m 755 LDView $RPM_BUILD_ROOT/usr/local/bin/LDView
install -m 644 ../Help.html $RPM_BUILD_ROOT/usr/local/share/ldview/Help.html
install -m 644 ../Readme.txt $RPM_BUILD_ROOT/usr/local/share/ldview/Readme.txt
install -m 644 ../ChangeHistory.html $RPM_BUILD_ROOT/usr/local/share/ldview/ChangeHistory.html
install -m 644 ../license.txt $RPM_BUILD_ROOT/usr/local/share/ldview/license.txt
install -m 644 ../m6459.ldr $RPM_BUILD_ROOT/usr/local/share/ldview/m6459.ldr

%files
/usr/local/bin/LDView
/usr/local/share/ldview/Help.html
/usr/local/share/ldview/license.txt
/usr/local/share/ldview/ChangeHistory.html
/usr/local/share/ldview/m6459.ldr
/usr/local/share/ldview/Readme.txt

%clean
rm -rf $RPM_BUILD_ROOT

