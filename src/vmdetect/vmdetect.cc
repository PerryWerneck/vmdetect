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

 #include <config.h>
 #include <iostream>
 #include <vmdetect/virtualmachine.h>
 #include <iostream>
 #include <getopt.h>

 using namespace std;


 int main(int argc, char **argv) {

	static struct option options[] = {
		{ "probe",		no_argument,		0,	'p' },
	};

	try {

		int long_index =0;
		int opt;
		while((opt = getopt_long(argc, argv, "p", options, &long_index )) != -1) {

			switch(opt) {
			case 'p':
				return VirtualMachine() ? 0 : 1;
			}

		}

		cout << VirtualMachine() << endl;

	} catch(const exception &e) {

		cerr << endl << e.what() << endl;
		return -1;
	}

	return 0;

 }
