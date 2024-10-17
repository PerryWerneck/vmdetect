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
 #include <functional>
 #include <cstring>

 using namespace std;

 /// @brief Command-line arguments.
 static bool verbose = false;

 static const struct Worker {

	char short_arg;
	const char *long_arg;
	const bool required;
	const char *help;
	const std::function<bool(const char *argument)> method;

	Worker(char s, const char *l, const char *h, const bool r, const std::function<bool(const char *argument)> m)
		: short_arg{s}, long_arg{l}, required{r}, help{h}, method{m} {
	}
 } workers[] {
	{
		'v',"verbose",
		"Show virtual machine name ('Bare metal' if not virtual)",
		false,
		[](const char *) {
			verbose = true;
			return false;
		}
	},
	{
		'n',"name",
		"Show virtual machine name ('Bare metal' if not virtual)",
		false,
		[](const char *) {
			VirtualMachine vm{verbose};
			if(vm) {
				cout << vm.name() << endl;
			} else {
				cout << "Bare metal" << endl;
			}
			return false;
		}
	},
	{
		'i',"id",
		"Show CPU ID",
		false,
		[](const char *) {
			VirtualMachine vm{verbose};
			if(vm) {
				cout << ((int) vm.id()) << endl;
			} else {
				cout << "0" << endl;
			}
			return false;
		}
	},
	{
		'I',"interactive",
		"Interactive mode",
		false,
		[](const char *) {

			string cmdline;

			while(1) {

				cout << PACKAGE_NAME << ": ";
				getline(cin,cmdline);

				if(cmdline.empty() || !strcmp(cmdline.c_str(),"exit")) {
					break;
				} else if(!strcmp(cmdline.c_str(),"name")) {
					cout << VirtualMachine{verbose}.name() << endl;
				} else {
					cout << "'" << cmdline << "': " << strerror(ENOENT) << endl;
				}

			}

			return false;
		}
	},

 };

 int main(int argc, char **argv) {

	try {

		if(argc == 1) {
#ifdef DEBUG
			return VirtualMachine(true) ? 0 : 1;
#else
			return VirtualMachine() ? 0 : 1;
#endif // DEBUG
		}

		// Check command-line arguments.

		while(--argc) {

			bool found = false;
			const char *argument = *(++argv);

			if(!strncmp(argument,"--",2)) {

				argument += 2;
				const char *value = strchr(argument,'=');
				string name;
				if(value) {
					name.assign(argument,value-argument);
					value++;
				} else {
					name.assign(argument);
					value = "";
				}

				for(const Worker &worker : workers) {

					found = (strcmp(name.c_str(),worker.long_arg) == 0);
					if(found) {
						if(worker.method(value)) {
							return 0;
						}
						break;
					}

				}

			} else if(argument[0] == '-') {

				argument++;

				if(argument[1]) {
					cerr << "Unexpected argument" << endl;
					return -1;
				}

				for(const Worker &worker : workers) {

					found = (worker.short_arg == argument[0]);
					if(found) {

						const char *value = "";

						if(worker.required) {

							if(argc == 1) {
								cerr << "An argument is required" << endl;
								exit(-1);
							}

							value = *(++argv);
							--argc;

							if(value[0] == '-') {
								cerr << "An argument is required" << endl;
								exit(-1);
							}

						}

						if(worker.method(value)) {
							return 0;
						}
						break;

					}

				}

			}

		}

	} catch(const std::exception &e) {

		cerr << e.what() << endl;
		exit(-1);

	}

	return 0;

 }
