project(
	'wssetup', 
	['c','cpp'],
	version: '0.0.1',
	default_options : ['c_std=c11', 'cpp_std=c++17'],
	license: 'GPL-3.0-or-later',
)

cxx = meson.get_compiler('cpp')

#
# Compiler flags
#
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
conf_data = configuration_data()
conf_data.set_quoted('PACKAGE_NAME', 'vmdetect')
conf_data.set_quoted('PACKAGE_VERSION', '1.3.1')

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  conf_data.set('HAVE_UNISTD_H', 1)
endif

configure_file(output : 'config.h', configuration : conf_data)

includes_dir = include_directories('../src/include')

#
# Dependencies
#
lib_deps = [
	dependency('dbus-1')
]

lib_src = [
  'src/libvmdetect/common.cc',
  'src/libvmdetect/os/linux/virtualmachine.cc',
]

dynamic = shared_library(
  'vmdetect', 
  lib_src,
	install: true,
	dependencies: lib_deps,
	include_directories: includes_dir
)

static = static_library(
  'vmdetect', 
  lib_src,
	install: true,
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



