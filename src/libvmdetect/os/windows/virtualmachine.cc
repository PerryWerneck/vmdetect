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
#include <iostream>
#include <cstdlib>

#ifdef HAVE_WMI
	#include <wmi.hpp>
	#include <wmiclasses.hpp>
#endif // HAVE_WMI

using namespace std;

/*---[ Implement ]----------------------------------------------------------------------------------*/

#if defined(__i386__) || defined(__x86_64__)

enum CpuID : uint8_t {
	BARE_METAL,			///< @brief Running on bare metal
	VMWARE,				///< @brief Running on VMWare
	VPC,				///< @brief Running on Virtual PC
	BHIVE,				///< @brief Running on BHIVE
	XEN,				///< @brief Running on XEN
	KVM,				///< @brief Running on KVM
	QEMU,				///< @brief Running on QEMU
	LKVM,				///< @brief Running on LKVM
	VMM,				///< @brief Running on VMM

	UNKNOWN				///< @brief Running on Unknown virtual machine
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

#ifdef _WIN32
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

		rc = BARE_METAL;

#ifdef HAVE_WMI

		try {

			auto computer = Wmi::retrieveWmi<Wmi::Win32_ComputerSystemProduct>();

#ifdef DEBUG
			cout << "*** Vendor= '" << computer.Vendor << "'" << endl;
#endif // DEBUG

			if(!computer.Vendor.empty()) {

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

		} catch (const Wmi::WmiException &ex) {
			rc = UNKNOWN;
			cerr << "Wmi error: " << ex.errorMessage << ", Code: " << ex.hexErrorCode() << endl;
		}

#endif // HAVE_WMI

	}
#endif // _WIN32

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
	return string{virtual_machine_name()};
}

#else // !i386, !x86_64

VirtualMachine::operator bool() const {
	return false;
}

const std::string VirtualMachine::to_string() const {
	return "";
}

const std::string VirtualMachine::to_string() const {
	return "";
}

#endif // !i386, !x86_64




