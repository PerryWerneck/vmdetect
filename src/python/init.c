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

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

 #include <vmdetect/defs.h>
 #include <private/python.h>

 PyObject * virtualmachine_module = NULL;

 static void cleanup(PyObject *module);

 static PyMethodDef methods[] = {

	{
		"version",
		pyvm_get_module_version,
		METH_NOARGS,
		"Get package version"
	},

	{
		"revision",
		pyvm_get_module_revision,
		METH_NOARGS,
		"Get package revision"

	},

	{
		"name",
		pyvm_get_name,
		METH_NOARGS,
		"Get virtual machine name"

	},

	{
		"id",
		pyvm_get_id,
		METH_NOARGS,
		"Get virtual machine id"

	},

	{
		NULL,
		NULL,
		0,
		NULL
	}

};

static struct PyModuleDef definition = {
	PyModuleDef_HEAD_INIT,
	.m_name = "virtualmachine",					// name of module
	.m_doc = "Get info about virtual machine",	// module documentation, may be NUL
	.m_size = -1,								// size of per-interpreter state of the module or -1 if the module keeps state in global variables.
	.m_methods = methods,						// Module methods
	.m_free = (freefunc) cleanup
};

PyMODINIT_FUNC PyInit_virtualmachine(void)
{

    //
    // Initialize module.
    //
    Py_Initialize();

    virtualmachine_module = PyModule_Create(&definition);

    if(!virtualmachine_module)
		return NULL;

    return virtualmachine_module;
}

static void cleanup(PyObject *module) {


}

