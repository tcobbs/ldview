Summary: LDraw is an open standard for LEGO CAD programs
Name: ldraw
Group: Applications/Multimedia
Version: 2013.01
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
if [ ! -d ldraw ] ; then mkdir ldraw ; fi
cd ldraw
#wget -N http://www.ldraw.org/download/software/ldraw/ldraw027.zip
wget -N http://www.ldraw.org/library/updates/complete.zip

%build

%install
if [ ! -d $RPM_BUILD_ROOT/usr/share/ ] ; then
	mkdir -p $RPM_BUILD_ROOT/usr/share/
fi
cd $RPM_BUILD_ROOT/usr/share/
if [ -d ldraw ] ; then rm -rf ldraw ; fi
#unzip -q $RPM_SOURCE_DIR/ldraw/ldraw027.zip
unzip -q -o $RPM_SOURCE_DIR/ldraw/complete.zip
cd ldraw
if [ -d p ] ; then mv p P ; fi
if [ -d parts ] ; then mv parts PARTS ; fi

%files
/usr/share/ldraw

%clean
rm -rf $RPM_BUILD_ROOT

