Summary: LDraw is an open standard for LEGO CAD programs
Name:    ldraw
%if 0%{?suse_version} || 0%{?sles_version}
Group:   Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version} 
Group:   Graphics
%endif
%if 0%{?fedora} || 0%{?centos_version}
Group:   Amusements/Graphics
%endif
Version: 2018.02
Release: 1
License: CC-BY-2.0
URL:     http://www.ldraw.org
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRequires: wget, unzip
BuildRoot: %{_builddir}/%{name}
BuildArch: noarch

%description
LDraw is an open standard for LEGO CAD programs that allow the user to create virtual LEGO models and scenes.

%prep
cd $RPM_SOURCE_DIR
if [ ! -f complete.zip ] ; then
	wget -N https://library.ldraw.org/library/updates/complete.zip
fi

%build

%install
install -d $RPM_BUILD_ROOT/usr/share/
cd $RPM_BUILD_ROOT/usr/share/
unzip -q -o $RPM_SOURCE_DIR/complete.zip
cd ldraw
rm -f *.exe *.ovl *.bgi || true

%files
%if 0%{?sles_version} || 0%{?suse_version}
%defattr(-,root,root)
%endif
/usr/share/ldraw

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Wed Jun 6 2019 - pbartfai (at) stardust.hu 2018.2
- do not rename directory p and part to uppercase

* Mon Jan 9 2017 - pbartfai (at) stardust.hu 2016.1
- Changelog added
