project(
	'vmdetect', 
	['c','cpp'],
	version: '1.3.1',
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

compiler_flags_common = [
  '-fvisibility=hidden',
  '-fPIC'
	]

if enable_debug
  compiler_flags_common += [
	  '-DDEBUG=1'
  ]
else
  compiler_flags_common += [
	'-DNDEBUG=1'
  ]
endif

compiler_flags_common += [
  '-DHAVE_CONFIG_H=1'
]

global_cxxflags = cxx.get_supported_arguments(compiler_flags_common)

add_project_arguments(global_cxxflags, language: 'cpp')

add_project_arguments(
            cxx.get_supported_arguments(
                [
                    '-ggdb3',
                    '-ffat-lto-objects',
                    '-fvisibility=hidden',
                    '-fPIC'
                ],
            ),
            language: 'cpp',
        )

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

if cxx.compiles('#include <unistd.h>', name : 'unistd.h')
  app_conf.set('HAVE_UNISTD_H', 1)
endif

config_src = [
  configure_file(
      output : 'config.h', 
      configuration : app_conf
  )
]

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

if host_machine.system() == 'windows'
  sdk_conf.set('REQUIRES','')
  sdk_conf.set('DYNLIBS','-lvmdetect.dll')
  sdk_conf.set('STATICLIBS','-lvmdetect')
else
  sdk_conf.set('REQUIRES','dbus-1')
  sdk_conf.set('DYNLIBS','-lvmdetect')
  sdk_conf.set('STATICLIBS','-l:libvmdetect.a')
endif

configure_file(
  input : 'sdk/dynamic.pc.in',
  output : meson.project_name() + '.pc',
  install: true,
  install_dir: get_option('libdir') + '/pkgconfig',
  configuration : sdk_conf
)

configure_file(
  input : 'sdk/static.pc.in',
  output : meson.project_name() + '-static.pc',
  install: true,
  install_dir: get_option('libdir') + '/pkgconfig',
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
	lib_deps += [
	  dependency('wmi++')
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
dynamic = shared_library(
  'vmdetect', 
  config_src + lib_src,
	install: true,
	version : libversion,
	gnu_symbol_visibility: 'hidden',
	dependencies: lib_deps,
	include_directories: includes_dir
)

static = static_library(
  'vmdetect', 
  config_src + lib_src,
	install: true,
	dependencies: lib_deps,
	include_directories: includes_dir
)

executable(
	'vmdetect',
	config_src + [ 'src/vmdetect/vmdetect.cc' ],
	install: true,
	link_with : dynamic,
	include_directories: includes_dir
)

install_subdir('src/include/vmdetect', install_dir : get_option('includedir'))



