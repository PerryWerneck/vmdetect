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

 #include <config.h>
 #include <stdexcept>
 #include <vmdetect/virtualmachine.h>
 #include <cstring>
 #include <string>
 #include <stdint.h>

 #ifdef HAVE_WMI
	#include <wmi.hpp>
	#include <wmiclasses.hpp>
 #endif // HAVE_WMI

 using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 std::string VirtualMachine::name() const {

	// Reference: (https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html)
	string virtualization;

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

#ifdef HAVE_WMI
	{
		auto computer = Wmi::retrieveWmi<Wmi::Win32_ComputerSystemProduct>();
		if(!computer.Vendor.empty()) {
			return computer.Vendor;
		}
	}
#endif // HAVE_WMI

	if(cpuid != UNKNOWN) {
		for(size_t ix = 0; ix < (sizeof(keys)/sizeof(keys[0])); ix++) {
			if(cpuid == keys[ix].id) {
				return keys[ix].name;
			}
		}
	}

	return "Unknown";

 }

 #ifdef _MSC_VER

	VirtualMachine::CpuID VirtualMachine::id() const {

		#error Incomplete

	}

 #else

	// http://git.annexia.org/?p=virt-what.git;a=tree
	static unsigned int cpuid(unsigned int eax, char *sig) {

		unsigned int *sig32 = (unsigned int *) sig;

		asm volatile (
			"xchgl %%ebx,%1; xor %%ebx,%%ebx; cpuid; xchgl %%ebx,%1"
			: "=a" (eax), "+r" (sig32[0]), "=c" (sig32[1]), "=d" (sig32[2])
			: "0" (eax)
		);
		sig[12] = 0;

		return eax;

	}

	VirtualMachine::CpuID VirtualMachine::id() const {

		CpuID rc = BARE_METAL;
		char sig[13];

		unsigned int base = 0x40000000, leaf = base;
		unsigned int max_entries;

		memset (sig, 0, sizeof sig);
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

		#ifdef HAVE_WMI
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
		#endif // HAVE_WMI

		}

		return rc;

	}

 #endif // _MSC_VER




