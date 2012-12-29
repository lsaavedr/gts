%define alphatag %(date +%Y%m%d)
%define current %(pkg-config gts --modversion --silence-errors)

Summary: GNU Triangulated Surface Library (development snapshot)
Name: gts-snapshot
%if "%{current}" == ""
Version: 0.7.6
%else
Version: %{current}
%endif
Release: 8.%{alphatag}cvs%{?dist}
License: GPLv2
%if 0%{?suse_version}
Group: Productivity/Scientific/Other
%else
Group: Applications/Engineering
%endif
URL: http://gts.sourceforge.net
Packager: Matthieu Castellazzi <m.castellazzi@niwa.co.nz>
Source0: gts-snapshot.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if 0%{?suse_version}
Requires: libnetpbm
BuildRequires: gtkdoc >= 1.3-4, sgml-skel, netpbm
%if 0%{?suse_version} >= 1110
BuildRequires: libnetpbm-devel
%endif
%else
Requires: netpbm
BuildRequires: gtk-doc >= 1.3-4, netpbm-devel
%endif

# For both distros
Requires: glib2
BuildRequires: glib2-devel

%package devel
Summary: Development files for gts
%if 0%{?suse_version}
Group: Productivity/Scientific/Other
%else
Group: Applications/Engineering
%endif
Requires: glib2-devel
Requires: %{name} = %{version}-%{release}

%package doc
Summary: Documentation for GTS (development snapshot)
%if 0%{?suse_version}
Group: Productivity/Scientific/Other
%else
Group: Applications/Engineering
%endif
Requires: lynx

%description
GTS includes a number of useful functions to deal with triangulated
surfaces including, but not limited to, multi-resolution models,
Delaunay and Constrained Delaunay triangulations, set operations on
surfaces (intersection, union etc ...), bounding-boxes trees for
efficient collision and intersection detection, triangle strips and
binary heaps.
     
%description devel
This package contains the gts header files and libs.

%description doc
This package contains the GTS Reference, in HTML format.


%prep
%setup -q -n gts-snapshot

%build
RPM_OPT_FLAGS="$RPM_OPT_FLAGS -fPIC -DPIC"
if [ -x ./configure ]; then
    CFLAGS="$RPM_OPT_FLAGS" ./configure \
	--prefix=%{_prefix} \
	--disable-dependency-tracking \
	--libdir=%{_prefix}/%_lib \
	--mandir=%{_mandir} \
	--disable-static
else
    CFLAGS="$RPM_OPT_FLAGS" sh autogen.sh \
	--prefix=%{_prefix} \
	--disable-dependency-tracking \
        --libdir=%{_prefix}/%_lib \
	--mandir=%{_mandir} \
	--disable-static
fi

%{__make}
%{__make} -C doc scan templates sgml html


%install
rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

# Fedora compatible changes, won't hurt SuSE
mv -f $RPM_BUILD_ROOT%{_bindir}/delaunay $RPM_BUILD_ROOT%{_bindir}/gtsdelaunay 
mv -f $RPM_BUILD_ROOT%{_bindir}/happrox $RPM_BUILD_ROOT%{_bindir}/gtshapprox
mv -f $RPM_BUILD_ROOT%{_bindir}/transform $RPM_BUILD_ROOT%{_bindir}/gtstransform

# Comply shared library policy
find $RPM_BUILD_ROOT -name *.la -exec rm -f {} \;

# Comply static build policy
find $RPM_BUILD_ROOT -name *.a -exec rm -f {} \;


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
%doc COPYING NEWS README TODO
%{_bindir}/gtsdelaunay
%{_bindir}/gts2dxf
%{_bindir}/gts2oogl
%{_bindir}/gts2stl
%{_bindir}/gtscheck
%{_bindir}/gtscompare
%{_bindir}/gtstemplate
%{_bindir}/gtshapprox
%{_bindir}/stl2gts
%{_bindir}/gts2xyz
%{_bindir}/gtstransform
%{_libdir}/*.so.*
%{_mandir}/man1/*.1.*

%files devel
%defattr(-,root,root)
%doc COPYING
%{_bindir}/gts-config
%{_includedir}/*.h
%{_libdir}/*.so
%{_libdir}/pkgconfig/*.pc
%{_datadir}/aclocal/*.m4

%files doc
%defattr(-,root,root)
%doc COPYING doc/html/*.html

%changelog
* Tue Nov 03 2009 Matthieu Castellazzi <m.castellazzi@niwa.co.nz> - 8
- first attempt to use buildservice for gerris
- add sgml-skel and libnetpbm dependencies for the build
- add --silence-errors options to define %current

* Wed Jul 16 2008 Ivan Adam Vari <i.vari@niwa.co.nz> - 7
- Some minor changes found in debian packages

* Thu Jul 3 2008 Ivan Adam Vari <i.vari@niwa.co.nz> - 6
- Fixed typo in %files section (attr)
- Fixed typo for FC build requirement gtk-doc

* Thu May 15 2008 Ivan Adam Vari <i.vari@niwa.co.nz> - 5
- Added fedora 8 support for x86 (32bit only)
- Added doc package
- Removed libtool config files to comply with shared
  library policy
- Removed static build bits to comply with shared
  library policy
- Fixed dependencies

* Mon Jan 7 2008 Ivan Adam Vari <i.vari@niwa.co.nz> - 4
- Removed %{?_smp_mflags} from make due to intermittent
  build errors on some SMP systems

* Wed Dec 19 2007 Ivan Adam Vari <i.vari@niwa.co.nz> - 3
- Fixed creating RPM_BUILD_ROOT issue on some systems
- Fixed package (install) dependencies

* Fri Sep 28 2007 Ivan Adam Vari <i.vari@niwa.co.nz> - 2
- Added SLEx/SuSE compatibility
- Added 64bit compatibility
- Removed --disable-static flag

* Tue May 1 2007 Ivan Adam Vari <i.vari@niwa.co.nz> - 1
- Initial rpm release based on Fedora/Redhat Linux
