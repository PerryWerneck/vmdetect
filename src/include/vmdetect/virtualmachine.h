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

 #pragma once

 #include <vmdetect/defs.h>

 #ifdef __cplusplus

 #include <string>
 #include <cstdint>

 extern "C" {

 class VMDETECT_API VirtualMachine {
 private:
	bool console_output = false;

 public:

 	constexpr VirtualMachine(bool v = false) : console_output{v} {
 	}

 	inline bool verbose() const noexcept {
		return console_output;
 	}

 	inline void verbose(bool v) noexcept {
		console_output = v;
 	}

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

 	CpuID id() const;

	operator bool() const;
	std::string name() const;

#ifndef _MSC_VER
	inline std::string to_string() const {
		return name();
	}
#endif // !_MSC_VER

	static const VirtualMachine & getInstance();

 private:
	CpuID translate(const char *sig) const;

 };

 namespace std {

 #ifndef _MSC_VER
	inline string to_string(const VirtualMachine &vm) {
		return vm.name();
	}
 #endif // !_MSC_VER

	inline ostream& operator<< (ostream& os, const VirtualMachine &vm) {
		return os << vm.name();
	}

 }

 #endif // __cplusplus

 VMDETECT_API int virtual_machine_detected();
 VMDETECT_API const char * virtual_machine_name();

#ifdef __cplusplus
 }
#endif
