#
# spec file for package vmdetect
#
# Copyright (c) 2015 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (C) <2008> <Banco do Brasil S.A.>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Summary:		Detect when running under virtual machine
Name:			vmdetect
Version:		1.1
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/vmdetect.git

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	binutils
BuildRequires:	coreutils
BuildRequires:	gcc-c++ >= 5
BuildRequires:	pkgconfig(libsystemd)

%description
Simple command line tool designed to detect when running under virtual machine.

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

%package -n lib%{name}%{_libvrs}
Summary:    Core library for %{name}
Group:      Development/Libraries/C and C++

%description -n lib%{name}%{_libvrs}
C++ library designed to detect when running under virtual machine.

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

%package devel
Summary:    C++ development files for lib%{name}
Requires:   lib%{name}%{_libvrs} = %{version}
Group:      Development/Libraries/C and C++

%description devel
Header files for the %{name} library.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup

NOCONFIGURE=1 \
	./autogen.sh

%configure 

%build
make all

%install
%makeinstall

%files
%defattr(-,root,root)
%{_bindir}/vmdetect

%files -n lib%{name}%{_libvrs}
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_libdir}/lib%{name}.so.*

%files devel
%defattr(-,root,root)
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h

%post -n lib%{name}%{_libvrs} -p /sbin/ldconfig
%postun -n lib%{name}%{_libvrs} -p /sbin/ldconfig

%changelog

