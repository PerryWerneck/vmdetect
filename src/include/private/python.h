/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2023 Perry Werneck <perry.werneck@gmail.com>
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
  * @brief Brief description of this source.
  */

 #pragma once

 #include <vmdetect/defs.h>

 #define PY_SSIZE_T_CLEAN
 #include <Python.h>

 #ifdef __cplusplus

 #include <vmdetect/virtualmachine.h>

 using namespace std;

 #include <functional>

 extern "C" {
 #endif // __cplusplus

 extern VMDETECT_PRIVATE PyObject * vmdetect_module;

 VMDETECT_PRIVATE PyObject * pyvm_get_module_version(PyObject *, PyObject *);
 VMDETECT_PRIVATE PyObject * pyvm_get_module_revision(PyObject *, PyObject *);
 VMDETECT_PRIVATE PyObject * pyvm_get_name(PyObject *, PyObject *);
 VMDETECT_PRIVATE PyObject * pyvm_get_id(PyObject *, PyObject *);

 #ifdef __cplusplus

 VMDETECT_PRIVATE PyObject * call(PyObject *self, const std::function<PyObject * ()> &worker);

 }
 #endif // __cplusplus
