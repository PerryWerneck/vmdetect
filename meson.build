# Maintainer: Perry Werneck <perry.werneck@gmail.com>

# SPDX-License-Identifier: LGPL-3.0-or-later 

# Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project(
	'vmdetect', 
	['c','cpp'],
	version: '1.3.4',
	default_options : ['c_std=c11', 'cpp_std=c++17'],
	license: 'GPL-3.0-or-later',
)

project_description = 'Detect when running under virtual machine'

#
# Versioning
#
pkg_version = meson.project_version()
version_array = pkg_version.split('.')
pkg_major_version = version_array[0].to_int()
pkg_minor_version = version_array[1].to_int()
pkg_micro_version = version_array[2].to_int()

libversion = '@0@.@1@'.format(pkg_major_version,pkg_minor_version)

#
# Compiler flags
#
cxx = meson.get_compiler('cpp')
enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

linker_flags = [
]

compiler_flags = [
  '-fvisibility=hidden',
  '-fPIC',
  '-ggdb3',
  '-ffat-lto-objects',
  '-fvisibility=hidden',
  '-fPIC'
	]

if enable_debug
  compiler_flags += [
	  '-DDEBUG=1'
  ]
else
  compiler_flags += [
	'-DNDEBUG=1'
  ]
endif

compiler_flags += [
  '-DHAVE_CONFIG_H=1'
]

add_project_arguments(cxx.get_supported_arguments(compiler_flags), language: 'cpp')

#
# Configuration
#
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)
app_conf.set('PACKAGE_VERSION_MAJOR', pkg_major_version)
app_conf.set('PACKAGE_VERSION_MINOR', pkg_minor_version)
app_conf.set('PACKAGE_VERSION_MICRO', pkg_micro_version)

app_conf.set('PRODUCT_NAME', meson.project_name())
app_conf.set('PRODUCT_VERSION', meson.project_version())

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

dbus = dependency('dbus-1',required:false)
if dbus.found()
  app_conf.set('HAVE_DBUS', 1)  
endif

config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

lib_includes = ['src/include']

#
# Dependencies
#
lib_deps = [
  dbus
]

lib_src = [
  'src/libvmdetect/common.cc'
]

app_src = [ 
  'src/vmdetect/vmdetect.cc' 
]

if host_machine.system() == 'windows'

  # https://mesonbuild.com/Windows-module.html
  windows = import('windows')
  resources = windows.compile_resources(
    configure_file(
      input : 'src/libvmdetect/os/windows/resources.rc.in',
      output : 'resources.rc',
      configuration : app_conf
    )
  )

  lib_src += resources
  app_src += resources

  lib_src += [
    'src/libvmdetect/os/windows/virtualmachine.cc'
  ]

  wmi = dependency('wmi++',required:false)
  if wmi.found()
    lib_deps += [
      dependency('wmi++')
    ]
  else
    lib_src += [
      'wmi/diaa_sami_comsupp/diaa_sami_comsupp.cpp',
      'wmi/src/wmi.cpp',
      'wmi/src/wmiresult.cpp',
    ]
    lib_includes += [ 'wmi/include' ]
    if cxx.get_id() != 'msvc'
      linker_flags += [ '-lwbemuuid', '-loleaut32', '-lole32' ]
    endif
  endif

else
  lib_src += [
    'src/libvmdetect/os/linux/virtualmachine.cc'
  ]

endif

includes_dir = include_directories(lib_includes)

#
# SDK
# https://mesonbuild.com/Pkgconfig-module.html
#
if cxx.get_id() != 'msvc'

  pkg = import('pkgconfig')

  if host_machine.system() == 'windows'

    pkg.generate(
      name: meson.project_name(),
      description: project_description,
      libraries: [ '-lvmdetect.dll' ]
    )

    pkg.generate(
      name: meson.project_name() + '-static',
      description: project_description,
      libraries: [ '-lvmdetect' ],
      requires: [ lib_deps ],
    )

  else

    pkg.generate(
      name: meson.project_name(),
      description: project_description,
      libraries: [ '-lvmdetect' ]
    )

    pkg.generate(
      name: meson.project_name() + '-static',
      description: project_description,
      libraries: [ '-l:libvmdetect.a' ],
      requires: [ lib_deps ],
    )

  endif

endif

#
# Targets
#
static = static_library(
  'vmdetect', 
  config_src + lib_src,
  install: true,
  dependencies: lib_deps,
  include_directories: includes_dir,
  link_args: linker_flags
)

if cxx.get_id() == 'msvc'

  executable(
    'vmdetect',
    config_src + app_src,
    install: true,
    link_with : static,
    include_directories: includes_dir,
    link_args: linker_flags
  )

else

  dynamic = shared_library(
    'vmdetect', 
    config_src + lib_src,
    install: true,
    version : libversion,
    gnu_symbol_visibility: 'hidden',
    dependencies: lib_deps,
    include_directories: includes_dir,
    link_args: linker_flags
  )

  executable(
    'vmdetect',
    config_src + app_src,
    install: true,
    link_with : dynamic,
    include_directories: includes_dir,
    link_args: linker_flags
  )

endif

#
# Install headers
#
install_headers(
  'src/include/vmdetect/defs.h',
  'src/include/vmdetect/virtualmachine.h',
  subdir: 'vmdetect'  
)

#
# Check for python
#
if not meson.is_cross_build()
  py = dependency('python3',required:false,method:'pkg-config')
  if py.found()
    import('python').find_installation().extension_module(
      'virtualmachine',
      config_src + [ 'src/python/init.c',  'src/python/get.cc' ],
      install: true,
      install_tag: 'python',
      link_with : static,
      dependencies: lib_deps + [ py ],
      include_directories: includes_dir,
      link_args: linker_flags
    )
  endif
endif



