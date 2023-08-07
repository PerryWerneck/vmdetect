#!/usr/bin/python
#-*- coding: utf-8

from setuptools import setup, Extension
import platform
import os
import glob

include_dirs = ['src/include']
library_dirs = []
extra_link_args = []
library_names = [ ]
src_files = [ ]

for filename in glob.glob("src/libdmiget/*.cc"):
	src_files.append(filename)
	
for filename in glob.glob("src/libdmiget/node/*.cc"):
	src_files.append(filename)
	
for filename in glob.glob("src/libvmdetect/*.cc"):
	src_files.append(filename)
	
for filename in glob.glob("src/python/*.cc"):
	src_files.append(filename)

for filename in glob.glob("src/python/*.c"):
	src_files.append(filename)

if platform.system() == 'Windows':

	include_dirs.append('src/libvmdetect/os/windows/wmi/include')
	
	for filename in glob.glob("src/libvmdetect/os/windows/*.cc"):
		src_files.append(filename)

	for filename in glob.glob("src/libvmdetect/os/windows/wmi/src/*.cpp"):
		src_files.append(filename)

	for filename in glob.glob("src/libvmdetect/os/windows/wmi/diaa_sami_comsupp/*.cpp"):
		src_files.append(filename)

else:

	library_names.append('systemd')
	
	for filename in glob.glob("src/libvmdetect/os/linux/*.cc"):
		src_files.append(filename)

virtualmachine = Extension(
		'virtualmachine',
		include_dirs = include_dirs,
		libraries = library_names,
		library_dirs=library_dirs,
		extra_link_args=extra_link_args,
		sources=src_files
)

package_version='0.1'
with open(r'configure.ac', 'r') as fp:
    lines = fp.readlines()
    for line in lines:
        if line.find('AC_INIT') != -1:
            package_version = line.split('[')[2].split(']')[0].strip()
            break;
            
setup ( name = 'virtualmachine',
	version = package_version,
	description = 'Python library to identify virtual machine.',
	author = 'Perry Werneck',
	author_email = 'perry.werneck@gmail.com',
	url = 'https://github.com/PerryWerneck/vmdetect',
	long_description = '''
This is an extension to identify virtual machine on python applications.
''',
	ext_modules = [ virtualmachine ])

