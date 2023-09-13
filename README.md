# vmdetect

C++/python libraries and simple command line tool designed to detect when running under virtual machine.

Based py_vmdetect sources from https://github.com/kepsic/py_vmdetect

![Platform: Linux/Windows](https://img.shields.io/badge/Platform-Linux/Windows-blue.svg)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![CodeQL](https://github.com/PerryWerneck/vmdetect/workflows/CodeQL/badge.svg?branch=master)
[![build result](https://build.opensuse.org/projects/home:PerryWerneck:udjat/packages/vmdetect/badge.svg?type=percent)](https://build.opensuse.org/package/show/home:PerryWerneck:udjat/vmdetect)
[![PyPI version](https://badge.fury.io/py/virtualmachine.svg)](https://badge.fury.io/py/virtualmachine)

## Installation

### Pre build packages

You can download installation package for supported linux distributions in [Open Build Service](https://software.opensuse.org/download.html?project=home%3APerryWerneck%3Audjat&package=vmdetect)

[<img src="https://raw.githubusercontent.com/PerryWerneck/pw3270/develop/branding/obs-badge-en.svg" alt="Download from open build service" height="80px">](https://software.opensuse.org/download.html?project=home%3APerryWerneck%3Audjat&package=vmdetect)
[<img src="https://github.com/PerryWerneck/PerryWerneck/blob/master/badges/msys-msvc-python-badge.svg" alt="Download from githut" height="80px">](https://github.com/PerryWerneck/vmdetect/releases)
[<img src="https://raw.githubusercontent.com/PerryWerneck/PerryWerneck/master/badges/pypi-badge.svg" alt="Download from pypi" height="80px">](https://pypi.org/project/virtualmachine)


## Examples:

### Command line

```shell
vmdetect
echo $?
```

```shell
vmdetect -n
Bare Metal
```

```shell
vmdetect -i
0
```

### Python

```python
import virtualmachine
print(virtualmachine.name())
```

```python
import virtualmachine
print(virtualmachine.id())
```

### C++

```C
#include <vmdetect/virtualmachine.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	VirtualMachine vm;

	if(vm) {
		cout << "Running on '" << vm << "' virtual machine" << endl;
		return 1;
	}

	return 0;
}
```


