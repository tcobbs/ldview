Summary: LDraw is an open standard for LEGO CAD programs
Name: ldraw
Group: Applications/Multimedia
Version: 2015.02
Release: 1
License: undefined
URL: http://www.ldraw.org
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRequires: wget, unzip
BuildRoot: %{_builddir}/%{name}
BuildArch: noarch

%description
LDraw is an open standard for LEGO CAD programs that allow the user to create virtual LEGO models and scenes.

%prep
cd $RPM_SOURCE_DIR
if [ ! -f complete.zip ] ; then
	wget -N http://www.ldraw.org/library/updates/complete.zip
fi

%build

%install
install -d $RPM_BUILD_ROOT/usr/share/
cd $RPM_BUILD_ROOT/usr/share/
unzip -q -o $RPM_SOURCE_DIR/complete.zip
cd ldraw
if [ -d p ] ; then mv p P ; fi
if [ -d parts ] ; then mv parts PARTS ; fi
rm -f *.exe *.ovl *.bgi || true

%files
/usr/share/ldraw

%clean
rm -rf $RPM_BUILD_ROOT

