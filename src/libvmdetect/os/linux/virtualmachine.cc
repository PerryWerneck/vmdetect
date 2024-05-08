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

 using namespace std;

 #ifdef HAVE_SYSTEMD
	#include <systemd/sd-bus.h>
 #endif // HAVE_SYSTEMD

/*---[ Implement ]----------------------------------------------------------------------------------*/

 std::string VirtualMachine::name() const {

	// Reference: (https://www.freedesktop.org/software/systemd/man/org.freedesktop.systemd1.html)
	string virtualization;

#if defined(HAVE_SYSTEMD)

	struct SystemBus {

		bool verbose;
		sd_bus *ptr = NULL;

		SystemBus(bool v) : verbose{v} {
			int rc = sd_bus_default_system(&ptr);
			if(rc < 0) {
				if(verbose) {
					cout << PACKAGE_NAME << "\tError " << rc << " getting system bus" << endl;
				}
				ptr = NULL;
			}
			if(verbose) {
				cout << PACKAGE_NAME << "\tGot system bus on socket " << sd_bus_get_fd(ptr) << endl;
			}
		}

		~SystemBus() {

			sd_bus_flush(ptr);

			if(verbose) {
				cout << PACKAGE_NAME << "\tReleasing system bus from socket " << sd_bus_get_fd(ptr) << endl;
			}

			sd_bus_unrefp(&ptr);
		}

	};

	struct BusMessage {
		sd_bus_message *ptr = NULL;
		~BusMessage() {
			sd_bus_message_unrefp(&ptr);
		}
	};

	SystemBus bus{console_output};
	if(bus.ptr) {

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
			BusMessage response;

			int rc = sd_bus_call_method(
					bus.ptr,                   				// On the System Bus
					"org.freedesktop.systemd1",				// Service to contact
					"/org/freedesktop/systemd1", 			// Object path
					"org.freedesktop.DBus.Properties",		// Interface name
					"Get",									// Method to be called
					&error,									// object to return error
					&response.ptr,							// Response message on success
					"ss",
					"org.freedesktop.systemd1.Manager",
					"Virtualization"
			);

			if(rc < 0) {
				string err = error.message;
				sd_bus_error_free(&error);
				throw runtime_error(err);;

			} else if(response.ptr) {

				char *text = NULL;

				if(sd_bus_message_read(response.ptr, "v", "s", &text) < 0) {
					throw runtime_error("Can't parse systemd virtualization response");
				} else if(text && *text) {
					virtualization = text;
				}

			}

		} catch(...) {
			throw;
		}

		return virtualization;

	}
#endif // HAVE_SYSTEMD

	// Cant get from SystemD, fallback

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

	if(cpuid != UNKNOWN) {
		for(size_t ix = 0; ix < (sizeof(keys)/sizeof(keys[0])); ix++) {
			if(cpuid == keys[ix].id) {
				return keys[ix].name;
			}
		}
	}

	return "Unknown";

 }

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

	return rc;

 }



