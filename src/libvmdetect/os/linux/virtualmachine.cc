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

using namespace std;

#ifdef HAVE_SYSTEMD
	#include <systemd/sd-bus.h>
#endif // HAVE_SYSTEMD

/*---[ Implement ]----------------------------------------------------------------------------------*/

#ifdef HAVE_SYSTEMD

VirtualMachine::operator bool() const {
	return !to_string().empty();
}

const std::string VirtualMachine::to_string() const {

	// Reference: (https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html)
	string virtualization = "";

	sd_bus * bus = NULL;
	if(sd_bus_open_system(&bus) < 0) {
		throw runtime_error("Can't open system bus");
	}

	try {

		/*
			dbus-send \
				--session \
				--dest=org.freedesktop.systemd1 \
				--print-reply \
				"/org/freedesktop/systemd1" \
				"org.freedesktop.DBus.Properties.Get" \
				string:"org.freedesktop.systemd1.Manager" \
				string:"Virtualization"
		*/

		sd_bus_error error = SD_BUS_ERROR_NULL;
		sd_bus_message *response = NULL;

		int rc = sd_bus_call_method(
				bus,                   					// On the System Bus
				"org.freedesktop.systemd1",				// Service to contact
				"/org/freedesktop/systemd1", 			// Object path
				"org.freedesktop.DBus.Properties",		// Interface name
				"Get",									// Method to be called
				&error,									// object to return error
				&response,								// Response message on success
				"ss",
				"org.freedesktop.systemd1.Manager",
				"Virtualization"
		);

		if(rc < 0) {
			string err = error.message;
			sd_bus_error_free(&error);
			throw runtime_error(err);;

		} else if(response) {

			char *text = NULL;

			if(sd_bus_message_read(response, "v", "s", &text) < 0) {
				sd_bus_message_unref(response);
				throw runtime_error("Can't parse systemd virtualization response");
			} else if(text && *text) {
				virtualization = text;
			}

			sd_bus_message_unref(response);

		}

	} catch(...) {
		sd_bus_flush_close_unref(bus);
		throw;
	}

	sd_bus_flush_close_unref(bus);

	return virtualization;

}

VMDETECT_API const char * virtual_machine_name() {
	static const std::string name{VirtualMachine().to_string()};
	return name.c_str();
}

#elif defined(__i386__) || defined(__x86_64__)

enum CpuID : uint8_t {
	BARE_METAL,			///< @brief Running on bare metal
	VMWARE,				///< @brief Running on VMWare
	VPC,				///< @brief Running on Virtual PC
	BHIVE,				///< @brief Running on BHIVE
	XEN,				///< @brief Running on XEN
	KVM,				///< @brief Running on KVM
	QEMU,				///< @brief Running on QEMU
	LKVM,				///< @brief Running on LKVM
	VMM					///< @brief Running on VMM
};

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

static CpuID translate(const char *sig) {

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

static CpuID getID() {

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

	return rc;

}

VirtualMachine::operator bool() const {
	return getID() != BARE_METAL;
}

VMDETECT_API const char * virtual_machine_name() {

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

	CpuID id = getID();
	if(id == BARE_METAL)
		return "";

	for(size_t ix = 0; ix < (sizeof(keys)/sizeof(keys[0])); ix++) {
		if(id == keys[ix].id) {
			return keys[ix].name;
		}
	}

	return "Unknown";
}

const std::string VirtualMachine::to_string() const {
	return virtual_machine_name();
}

#else // !i386, !x86_64

VirtualMachine::operator bool() const {
	return false;
}

const std::string VirtualMachine::to_string() const {
	return "";
}

VMDETECT_API const char * virtual_machine_name() {
	return "";
}

#endif // !i386, !x86_64




