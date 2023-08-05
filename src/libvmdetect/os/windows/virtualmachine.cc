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

/**
 * @file virtualmachine.cc
 *
 * @brief Detect virtual machine.
 *
 * References:
 *
 * <https://www.codeproject.com/Articles/9823/Detect-if-your-program-is-running-inside-a-Virtual>
 * <https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html>
 * <https://people.redhat.com/~rjones/virt-what/>
 * <http://artemonsecurity.com/vmde.pdf>
 * <http://git.annexia.org/?p=virt-what.git;a=tree>
 *
 * SystemD References:
 *
 * <https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html>

 * @author perry.werneck@gmail.com
 *
 */

 #ifdef HAVE_CONFIG_H
 	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <stdexcept>
 #include <vmdetect/virtualmachine.h>
 #include <cstring>
 #include <string>
 #include <stdint.h>
 #include <iostream>

 #ifdef _MSC_VER

	#include <intrin.h>

	#pragma comment(lib, "wbemuuid.lib")
	#pragma comment(lib, "oleaut32")
	#pragma comment(lib, "ole32")

 #endif // _MSC_VER

 #include <wmi.hpp>
 #include <wmiclasses.hpp>

 using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 std::string VirtualMachine::name() const {

	// Reference: (https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html)
	static const struct Key {
		CpuID	  	  id;
		const char	* name;
	} keys[] = {
		{ VMWARE,	"VMware"		},
		{ VPC,		"Microsoft Hv"	},
		{ BHIVE,	"bhyve"			},
		{ XEN,		"Xen"			},
		{ KVM,		"KVM"			},
		{ QEMU,		"QEMU"			},
		{ LKVM,		"LKVM"			},
		{ VMM,		"OpenBSDVMM58"	}
	};

	CpuID cpuid = this->id();
	if(cpuid == BARE_METAL)
		return "";

	if(cpuid == VPC) {
		auto computer = Wmi::retrieveWmi<Wmi::Win32_ComputerSystemProduct>();
		if(!computer.Vendor.empty()) {
			return computer.Vendor;
		}
	}

	if(cpuid != UNKNOWN) {
		for(size_t ix = 0; ix < (sizeof(keys)/sizeof(keys[0])); ix++) {
			if(cpuid == keys[ix].id) {
				return keys[ix].name;
			}
		}
	}

	return "Unknown";

 }


 /*
 // https://stackoverflow.com/questions/1666093/cpuid-implementations-in-c
 // http://git.annexia.org/?p=virt-what.git;a=tree
 static unsigned int cpuid(unsigned int eax, char *sig) {

	uint32_t *sig32 = (uint32_t *) sig;

	asm volatile (
		"xchgl %%ebx,%1; xor %%ebx,%%ebx; cpuid; xchgl %%ebx,%1"
		: "=a" (eax), "+r" (sig32[0]), "=c" (sig32[1]), "=d" (sig32[2])
		: "0" (eax)
	);
	sig[12] = 0;

	return eax;

 }
 */

 static int cpuid(unsigned int i, char sig[13]) {

	// https://stackoverflow.com/questions/1666093/cpuid-implementations-in-c

	uint32_t regs[4];
	memset(sig,0,13);

#ifdef _MSC_VER

	__cpuid((int *)regs, (int)i);

#else

	asm volatile
      ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
       : "a" (i), "c" (0));
   // ECX is set to zero for CPUID function 4

#endif // _MSC_VER

	uint32_t *ptr = (uint32_t *) sig;

	*(ptr++) = regs[1];	// EBX
	*(ptr++) = regs[2];	// EDX
	*(ptr++) = regs[3];	// ECX
	sig[12] = 0;

	return regs[0];
 }

 VirtualMachine::CpuID VirtualMachine::id() const {

	CpuID rc = BARE_METAL;
	char sig[13];

	unsigned int base = 0x40000000, leaf = base;
	unsigned int max_entries;

	max_entries = cpuid (leaf, sig);
	rc = translate(sig);

	//
	// Most hypervisors only have information in leaf 0x40000000, but
	// upstream Xen contains further leaf entries (in particular when
	// used with Viridian [HyperV] extensions).  CPUID is supposed to
	// return the maximum leaf offset in %eax, so that's what we use,
	// but only if it looks sensible.
	//
	if (rc == BARE_METAL && max_entries > 3 && max_entries < 0x10000) {
		for (leaf = base + 0x100; leaf <= base + max_entries && rc == BARE_METAL; leaf += 0x100) {
			memset (sig, 0, sizeof sig);
			cpuid (leaf, sig);
			rc = translate(sig);
		}
	}

	if(rc == BARE_METAL) {
		return rc;
	}

	// https://stackoverflow.com/questions/498371/how-to-detect-if-my-application-is-running-in-a-virtual-machine

	#ifdef HAVE_WMI
	if(rc == VPC) {
		//
		// Recent Windows 10 build is returning VPC even on bare metal, try to use WMI to identify the real hypervisor
		// https://www.splunk.com/en_us/blog/tips-and-tricks/detecting-your-hypervisor-from-within-a-windows-guest-os.html
		//
		static const struct {
			CpuID		  id;
			const char	* name;
		} Manufacturers[] = {
			{ QEMU,		"QEMU" 					},
			{ XEN,		"XEN"					},
			{ VMWARE,	"VMWARE"				},
			{ VPC,	 	"MICROSOFT HYPER-V"		}
		};

		{
			auto computer = Wmi::retrieveWmi<Wmi::Win32_ComputerSystemProduct>();

			if(!computer.Vendor.empty()) {

				rc = BARE_METAL;

				for(size_t ix = 0; ix < computer.Vendor.size(); ix++) {
					computer.Vendor[ix] = toupper(computer.Vendor[ix]);
				}

				for(size_t ix = 0; ix < (sizeof(Manufacturers)/sizeof(Manufacturers[0])); ix++) {
					if(strstr(computer.Vendor.c_str(),Manufacturers[ix].name)) {
						rc = Manufacturers[ix].id;
						break;
					}
				}

			}
		}

	}
	#endif // HAVE_WMI

	return rc;

}




