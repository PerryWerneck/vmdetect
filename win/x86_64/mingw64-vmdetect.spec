#
# spec file for package mingw64-vmdetect
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
Name:			mingw64-vmdetect
Version:		1.0
Release:		0
License:		LGPL-3.0
Source:			vmdetect-%{version}.tar.xz
BuildArch:		noarch

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1)
%define _libvrs %{MAJOR_VERSION}_%{MINOR_VERSION}

URL:			https://github.com/PerryWerneck/vmdetect.git

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	autoconf >= 2.61
BuildRequires:	automake
BuildRequires:	libtool
BuildRequires:	coreutils

BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64-cross-binutils
BuildRequires:	mingw64-cross-gcc-c++ >= 9.0
BuildRequires:	mingw64-cross-pkg-config
BuildRequires:	mingw64-filesystem
BuildRequires:	mingw64-wmi++-devel

%description
Tool to identify virtual when the system is running in a virtual machine.

%package -n mingw64-libvmdetect%{_libvrs}
Summary:	Core library for %{name}
Group:		Development/Libraries/C and C++
Provides:	mingw64-libvmdetect
Provides:	mingw64(lib:vmdetect.dll)

%description -n mingw64-libvmdetect%{_libvrs}
C++ library to identify when the system is running in a virtual machine.

%package devel
Summary:	C++ development files for lib%{name}
Requires:	mingw64-libvmdetect%{_libvrs} = %{version}
Group:		Development/Libraries/C and C++

%description devel
Header files for the %{name} library.

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%setup -n vmdetect-%{version}

NOCONFIGURE=1 \
	./autogen.sh

%{_mingw64_configure}

%build
make all %{?_smp_mflags}

%install
%{_mingw64_make} DESTDIR=%{buildroot} install

%files
%doc README.md
%license LICENSE
%defattr(-,root,root)
%{_mingw64_bindir}/vmdetect.exe

%files -n mingw64-libvmdetect%{_libvrs}
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_mingw64_bindir}/*.dll

%files devel
%doc README.md
%license LICENSE
%defattr(-,root,root)
%{_mingw64_libdir}/pkgconfig/*.pc
%{_mingw64_libdir}/*.a
%dir %{_mingw64_includedir}/vmdetect
%{_mingw64_includedir}/vmdetect/*.h

%exclude %{_mingw64_datadir}/vmdetect

%changelog


