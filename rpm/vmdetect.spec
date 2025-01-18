#
# spec file for package vmdetect
#
# Copyright (c) 2024 SUSE LINUX GmbH, Nuernberg, Germany.
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

Summary:		Detect virtual machine environments
Name:			vmdetect
Version:		1.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/vmdetect.git

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	fdupes
BuildRequires:	gcc-c++ >= 5
BuildRequires:	pkgconfig(dbus-1)
BuildRequires:	meson >= 0.61.4

%description
A comprehensive toolkit for detecting virtual machine environments. 
Available as a cross-platform library (Python, C, C++) and command-line 
tool for Windows and Linux, it employs multiple techniques to reliably 
identify various virtualization platforms (e.g., VMware, VirtualBox, 
Hyper-V, QEMU).

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

%define MAJOR_VERSION %(echo %{version} | cut -d. -f1)
%define MINOR_VERSION %(echo %{version} | cut -d. -f2 | cut -d+ -f1) 
%define _libvrs %{MAJOR_VERSION}

%package -n lib%{name}%{_libvrs}
Summary:	Core library for %{name}
Group:		Development/Libraries/C and C++
Provides:	lib%{name}%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}

%description -n lib%{name}%{_libvrs}
A cross-platform library (Python, C, C++) for Windows and Linux, 
employing multiple techniques to reliably identify various 
virtualization platforms (e.g., VMware, VirtualBox, Hyper-V, QEMU).

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

%package devel
Summary:	C++ development files for lib%{name}
Requires:	lib%{name}%{MAJOR_VERSION}_%{MINOR_VERSION} = %{version}
Group:		Development/Libraries/C and C++

%description devel
Development files for %{name} a cross-platform library to identify virtual machines 
on Windows and Linux.

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

#---[ Build & Install ]-----------------------------------------------------------------------------------------------

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install

%files
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_bindir}/vmdetect

%files -n lib%{name}%{_libvrs}
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_libdir}/*.so.%{MAJOR_VERSION}.%{MINOR_VERSION}

%files devel
%defattr(-,root,root)
%doc README.md
%license LICENSE
%{_libdir}/*.so.%{MAJOR_VERSION}
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc
%dir %{_includedir}/%{name}
%{_includedir}/%{name}/*.h

%post -n lib%{name}%{_libvrs} -p /sbin/ldconfig
%postun -n lib%{name}%{_libvrs} -p /sbin/ldconfig

%changelog

