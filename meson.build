project(
	'vmdetect', 
	['c','cpp'],
	version: '1.3.1',
	default_options : ['c_std=c11', 'cpp_std=c++17'],
	license: 'GPL-3.0-or-later',
)

project_description = 'Detect when running under virtual machine'

#
# Compiler flags
#
cxx = meson.get_compiler('cpp')
enable_debug = get_option('debug') or get_option('buildtype').contains('debug')

compiler_flags_common = [
  '-fvisibility=hidden',
  '-fPIC'
	]

if enable_debug
  compiler_flags_common += [
    '-ggdb3',
	'-DDEBUG=1'
  ]
else
  compiler_flags_common += [
	'-DNDEBUG=1'
  ]
endif

global_cxxflags = cxx.get_supported_arguments(compiler_flags_common)

add_project_arguments(global_cxxflags, language: 'cpp')

#
# Configuration
#
app_conf = configuration_data()
app_conf.set_quoted('PACKAGE_NAME', meson.project_name())
app_conf.set_quoted('PACKAGE_VERSION', meson.project_version())
app_conf.set_quoted('PACKAGE_DESCRIPTION', project_description)

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

configure_file(
  output : 'config.h', 
  configuration : app_conf
)

#
# SDK
#
sdk_conf = configuration_data()
sdk_conf.set('PACKAGE_NAME', meson.project_name())
sdk_conf.set('PACKAGE_VERSION', meson.project_version())
sdk_conf.set('PACKAGE_DESCRIPTION', project_description)
sdk_conf.set('prefix',get_option('prefix'))
sdk_conf.set('exec_prefix',get_option('prefix'))
sdk_conf.set('includedir',get_option('includedir'))
sdk_conf.set('libdir',get_option('libdir'))

configure_file(
  input : 'sdk/dynamic.pc.in',
  output : 'vmdetect.pc',
  configuration : sdk_conf
)

configure_file(
  input : 'sdk/static.pc.in',
  output : 'vmdetect-static.pc',
  configuration : sdk_conf
)

includes_dir = include_directories('../src/include')

#
# Dependencies
#
lib_deps = [
]

lib_src = [
  'src/libvmdetect/common.cc'
]

if host_machine.system() == 'windows'
  lib_src += [
    'src/libvmdetect/os/windows/virtualmachine.cc'
  ]
else
  lib_src += [
    'src/libvmdetect/os/linux/virtualmachine.cc'
  ]
	lib_deps += [
	  dependency('dbus-1')
	]
endif

#
# Targets
#

# https://mesonbuild.com/Pkgconfig-module.html
dynamic = shared_library(
  'vmdetect', 
  lib_src,
	dependencies: lib_deps,
	include_directories: includes_dir
)

static = static_library(
  'vmdetect', 
  lib_src,
	dependencies: lib_deps,
	include_directories: includes_dir
)

executable(
	'vmdetect',
	[ 'src/vmdetect/vmdetect.cc' ],
	install: true,
	link_with : dynamic,
	include_directories: includes_dir
)



