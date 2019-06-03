#include "cpp_cli.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char ** argv) {
	using namespace cli;
	std::string filename = "a.out";
	int recursion_level = -1;
	char flag = '\0';
	std::string flag2 = "false";
	double probability_of_success = 0.0001;
	std::string standard_input_hyphen = "";
	size_t verbosity;
	bool help = false;
	std::vector<int> list_of_ints;
	std::vector<const char *> list_of_declarations;
	list_of_ints.reserve(30);

	// non_options is a vector of args that did not start with a hyphen, did not
	// start with a double hyphen, or came after the special argument "--".
	// non_options orders its elements by the same order they came in on the
	// command line.
	std::vector<const char *> non_options;

	// This scoping is just to automatically delete the Vars after
	// they are no longer necessary. They need to be in accessable in the same
	// scope as when hash is called.

	Parser p;
	char header[] = "This test program will demonstrate the basic functionality of this library. Other test programs will demonstrate more advances functionality. This is just going to be a really long test header to demonstrate that the function that will break this into lines will work, so I have to add a bunch of unecessary information after the first few sentences so I can make sure that the formatter works.";

	char usage[] = "[options/non-options]\n\nIt doesn't really matter what you put here since none of the positions matter.";

	char footer[] = "For more information, contact me at the.landfill.coding@gmail.com or on the github page. You could also put your version information stuff here, which would be cool.";
	
	p.set_header(header);
	p.set_usage(usage);
	p.set_footer(footer);

	// The help file path must be set or else the program will throw an
	// exception. The help file path must also be a valid, writable path.
	// "" just means current directory. Do not use relative paths unless
	// you only plan on running it from a single folder.
	p.set_help_file_path("");

	p.arg(filename, { "f", "file", "filename" }, "Determines the file to be read. In this program, though, it doesn't do anything.");
	p.arg(recursion_level, { "r", "recursion", "max-depth" }, "Determines the maximum level of recursion allowed before nothing happens because this is a test program.");
	p.arg(probability_of_success, { "p", "prob", "probability" }, "Sets the probability of this program working properly, which is mostly dependent on whether or not I forgot a minor syntax error.");
	p.repeated(verbosity, { "v" }, "This is a standard verbosity variable that is supposed to set increasing levels of verbosity, so -v would mean be a little verbose, -vvvv would mean be very verbose, etc.");

	// Value s set the variable to the third argument.
	// If --flag or -a is passed, flag will be set to 'a'. If --no-flag or
	// -b is passed, flag will be set to 'b'.
	p.value(flag, { "flag", "a" }, 'a', "This uses the improved Value syntax to make sure that multiple flags set a variable to the same flag. Notice that it is of 'char' type and that flag is not an array.");
	p.value(flag, { "no-flag", "b" }, 'b', "Same as --flag, -a, except it will set the flag variable to a different value.");
	p.value(flag, { "some-flag", "c" }, 'c', "Same as --flag, -a, except it will set the flag variable to a different value.");

	// Generally, the solitary hyphen flag is used to indicate that the
	// program should take in standard input. The library will tell you that
	// someone typed a hyphen, and it is up to you to determine what you
	// want to do with it. It is not necessary to do anything with it. If
	// Unless you do something with it, the hyphen will be ignored.
	p.arg(standard_input_hyphen, { "-" }, "This is just a standard input hyphen, which normally means that your program wants to take in user input from stdin. It's only here to demonstrate that this library can parse it.");

	// This will put any argument starting with "-l" or "--library" into
	// non_options, even though it's still an option, as its order matters.
	// The type doesn't matter as you're not setting a variable. You will
	// have to take template specializations into account, such as char.
	// You should also set takes_args to be true, or else its position does
	// not matter.
	p.ignored({ "l", "library" }, "Flag that corresponds to gcc's -l flag. It is a position dependent flag.");

	// Having the same flag refer to two different Vars will
	// cause the program to fail and tell you which flag.
	// Var<int> breaks_program(nullptr, { "l" }, false);

	p.value(help, { "h", "help" }, true, "Prints this help message and exits.");

	p.vector(list_of_ints, { "i", "list" }, "Just here to demonstrate that it can deal with vectors of arguments.");
	p.vector(list_of_declarations, { "D" }, "Just here to demonstrate that const char *'s work.");

	p.generate_help(argv[0]);

	// Non options must be declared outside the scope unless you don't
	// need to use them.
	non_options = p.parse(argc, argv);

	// This section just prints out the values to demonstrate that hash worked.
	if (help) {
		p.print_help();
	} else {
		std::cout << "filename:\t" << filename << std::endl;
		std::cout << "recursion:\t" << recursion_level << std::endl;
		std::cout << "flag:\t\t'" << flag << "'" << std::endl;
		std::cout << "flag2:\t\t" << flag2 << std::endl;
		std::cout << "probability_of_success:\t" << probability_of_success << std::endl;
		std::cout << "standard_input_hyphen:\t" << standard_input_hyphen << std::endl;
		std::cout << "verbosity:\t" << verbosity << std::endl;
		std::string list_of_ints_string;
		list_of_ints_string.reserve(100);
		for (size_t i = 0; i < list_of_ints.size(); i++) {
			list_of_ints_string += std::to_string(list_of_ints[i]);
			list_of_ints_string += ", ";
		}
		std::cout << "List of ints:\t[ " << list_of_ints_string.substr(0, list_of_ints_string.length() - 2) << " ]" << std::endl;
		std::cout << "List of ints size:\t" << std::to_string(list_of_ints.size()) << std::endl;
		std::string list_of_declarations_string = "";
		list_of_declarations_string.reserve(1000);
		for (size_t i = 0; i < list_of_declarations.size(); i++) {
			list_of_declarations_string += list_of_declarations[i];
			list_of_declarations_string += ", ";
		}
		std::cout << "List of declarations:\t[ " << list_of_declarations_string.substr(0, list_of_declarations_string.length() - 2) << " ]" << std::endl;
		for (size_t i = 0; i < non_options.size(); i++) {
			std::cout << "NON_OPTION " << i << ":\t" << non_options[i] << std::endl;
		}
	}

	return 0;
}
