project(
	'wmi', 
	['cpp'],
	version: '1.0.0',
	default_options : ['c_std=c11'],
)

wmi_src = [
  'diaa_sami_comsupp/diaa_sami_comsupp.cpp',
  'src/wmi.cpp',
  'src/wmiresult.cpp',
]

wmi_inc = include_directories('include')

wmi_args = [
  '-fvisibility=hidden',
  '-fPIC',
  '-ggdb3',
]

wmi_lib = static_library('wmi',
  wmi_src,
  include_directories: wmi_inc,
  c_args: wmi_args,
)

wmi_dep = declare_dependency(
  link_with: wmi_lib,
  include_directories: wmi_inc
)
