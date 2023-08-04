/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <iostream>
 #include <vmdetect/virtualmachine.h>
 #include <cstring>

 const VirtualMachine & VirtualMachine::getInstance() {
	static VirtualMachine instance;
	return instance;
 }

 VirtualMachine::operator bool() const {
	return !name().empty();
 }

 VirtualMachine::CpuID VirtualMachine::translate(const char *sig) const {

	static const struct Key {
		CpuID		  id;
		const char	* sig;
	} keys[] = {
		{ VMWARE,	"VMwareVMware"	},
		{ VPC,		"Microsoft Hv"	},
		{ BHIVE,	"bhyve bhyve"	},
		{ XEN,		"XenVMMXenVMM"	},
		{ KVM,		"KVMKVMKVM"		},
		{ QEMU,		"TCGTCGTCGTCG"	},
		{ LKVM,		"LKVMLKVMLKVM"	},
		{ VMM,		"OpenBSDVMM58"	}
	};

	for(size_t ix = 0; ix < (sizeof(keys)/sizeof(keys[0])); ix++) {
		if(!strcmp(sig,keys[ix].sig)) {
			return keys[ix].id;
		}
	}

	return BARE_METAL;

 }


 VMDETECT_API const char * virtual_machine_name() {
	static const std::string name{VirtualMachine().name()};
	return name.c_str();
 }

 VMDETECT_API int virtual_machine_detected() {

	try {

		if(VirtualMachine()) {
			return 1;
		}

	} catch(...) {

		return -1;

	}

	return 0;

 }
