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

 extern "C" {

 class VMDETECT_API VirtualMachine {
 public:
	operator bool() const;
	std::string name() const;

	static const VirtualMachine & getInstance();

 };

 #ifndef _MSC_VER
 namespace std {

	inline string to_string(const VirtualMachine &vm) {
		return vm.name();
	}

	inline ostream& operator<< (ostream& os, const VirtualMachine &vm) {
		return os << vm.name();
	}

 }
 #endif // !_MSC_VER

 #endif // __cplusplus

 VMDETECT_API int virtual_machine_detected();
 VMDETECT_API const char * virtual_machine_name();

#ifdef __cplusplus
 }
#endif
