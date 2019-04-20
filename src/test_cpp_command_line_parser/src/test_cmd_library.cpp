#include "cpp_cli.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char ** argv) {

	// sample variables that you want to initialize with default values set just
	// like you would in any program

	std::string filename = "a.out";
	int recursion_level = -1;
	char flag = '\0';
	std::string flag2 = "false";
	double probability_of_success = 0.0001;
	std::string standard_input_hyphen = "";
	char verbosity[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	bool help = false;
	std::vector<int> list_of_ints;
	list_of_ints.reserve(30);

	// non_options is a vector of args that did not start with a hyphen, did not
	// start with a double hyphen, or came after the special argument "--".
	// non_options orders its elements by the same order they came in on the
	// command line.
	std::vector<const char *> non_options;

	// This scoping is just to automatically delete the Command_Line_Vars after
	// they are no longer necessary. They need to be in accessable in the same
	// scope as when hash is called.
	{

		char header[] = "This test program will demonstrate the basic functionality of this library. Other test programs will demonstrate more advances functionality. This is just going to be a really long test header to demonstrate that the function that will break this into lines will work, so I have to add a bunch of unecessary information after the first few sentences so I can make sure that the formatter works.";

		char usage[] = "[options/non-options]\nIt doesn't really matter what you put here since none of the positions matter.";

		char footer[] = "For more information, contact me at the.landfill.coding@gmail.com or on the github page. You could also put your version information stuff here, which would be cool.";
		
		ARGS_PARSER::set_header(header);
		ARGS_PARSER::set_usage(usage);
		ARGS_PARSER::set_footer(footer);

		// The help file path must be set or else the program will throw an
		// exception. The help file path must also be a valid, writable path.
		// "" just means current directory. Do not use relative paths unless
		// you only plan on running it from a single folder.
		ARGS_PARSER::set_help_file_path("");


		// For every variable that you want users to be able to set at the
		// command line, create an instance of Command_Line_Var, which takes in
		// the address of the variable (usually, just the '&' address operator
		// is sufficient) and a list of all the flags that could affect it.
		// The last flag set determines the value of the variable in question.

		// For instance, if you have -bca -b, where -b, -c, and -a correspond to
		// mutually exclusive options (the mv command and the flags -i, -f, and
		// -n are the most accessible examples), the flag variable will be set
		// to "b". If you use the long options along with the short options,
		// then the last of the short and long options will happen.

		// If the string you pass into the Command_Line_Var constructor is one
		// letter, then it is a short option, otherwise it is a long option.

		// Short options are automatically assigned to single dashes while long
		// options are assigned to two dashes. Short options can be aglomerated
		// as long as none of them take arguments. Short options can take
		// arguments in the forms -aValue or -a Value.

		// The library will first try to break apart single dash arguments into
		// multiple short options, but if it cannot find an option for every
		// character, it will assume the argument is of the form -aValue. If this
		// issue causes you problems, use the syntax -a Value instead.

		// All long options have the syntax --long-option or --long-option=arg.
		// If the option doesn't take any arguments, it will set the variable
		// to the option itself. In this example, if --flag is used, it will set
		// the variable flag to "flag". --long-option=arg will set long option
		// to arg. If Command_Line_Var is used with a standard numeric type, it
		// will automatically convert from a string to that numeric type.
		// Negative numbers must be in the format -o-val or --long-option=-val.

		Command_Line_Var<std::string> file_var(filename, { "f", "file", "filename" }, true, "Determines the file to be read. In this program, though, it doesn't do anything.");
		Command_Line_Var<int> recursion_var(recursion_level, { "r", "recursion", "max-depth" }, true, "Determines the maximum level of recursion allowed before nothing happens because this is a test program.");
		Command_Line_Var<std::string> flag2_var(flag2, { "flag2", "d" }, false, "This is just a standard flag variable using some moderately deprecated syntax.");
		Command_Line_Var<double> probability_of_success_var(probability_of_success, { "p", "prob", "probability" }, true, "Sets the probability of this program working properly, which is mostly dependent on whether or not I forgot a minor syntax error.");
		Command_Line_Var<char> verbosity_var(verbosity, { "v" }, false, 20, "This is a standard verbosity variable that is supposed to set increasing levels of verbosity, so -v would mean be a little verbose, -vvvv would mean be very verbose, etc.");

		// Command_Line_Value s set the variable to the third argument.
		// If --flag or -a is passed, flag will be set to 'a'. If --no-flag or
		// -b is passed, flag will be set to 'b'.
		Command_Line_Value<char> flag_var0(flag, { "flag", "a" }, 'a', "This uses the improved Command_Line_Value syntax to make sure that multiple flags set a variable to the same flag. Notice that it is of 'char' type and that flag is not an array.");
		Command_Line_Value<char> flag_var1(flag, { "no-flag", "b" }, 'b', "Same as --flag, -a, except it will set the flag variable to a different value.");
		Command_Line_Value<char> flag_var2(flag, { "some-flag", "c" }, 'c', "Same as --flag, -a, except it will set the flag variable to a different value.");

		// Generally, the solitary hyphen flag is used to indicate that the
		// program should take in standard input. The library will tell you that
		// someone typed a hyphen, and it is up to you to determine what you
		// want to do with it. It is not necessary to do anything with it. If
		// Unless you do something with it, the hyphen will be ignored.
		Command_Line_Var<std::string> standard_input_hyphen_var(standard_input_hyphen, { "-" }, false, "This is just a standard input hyphen, which normally means that your program wants to take in user input from stdin. It's only here to demonstrate that this library can parse it.");

		// This will put any argument starting with "-l" or "--library" into
		// non_options, even though it's still an option, as its order matters.
		// The type doesn't matter as you're not setting a variable. You will
		// have to take template specializations into account, such as char.
		// You should also set takes_args to be true, or else its position does
		// not matter.
		Command_Line_Var<int> ignored_flag_var(nullptr, { "l", "library" }, true, "Flag that corresponds to gcc's -l flag. It is a position dependent flag.");

		// Having the same flag refer to two different Command_Line_Vars will
		// cause the program to fail and tell you which flag.
		// Command_Line_Var<int> breaks_program(nullptr, { "l" }, false);

		Command_Line_Value<bool> help_var(help, { "h", "help" }, true, "Prints this help message and exits.");

		Command_Line_Vector<int> list_of_ints_var(list_of_ints, { "i", "list" }, "Just here to demonstrate that it can deal with vectors of arguments.");

		ARGS_PARSER::generate_help(argv[0]);

		// Non options must be declared outside the scope unless you don't
		// need to use them.
		non_options = ARGS_PARSER::parse(argc, argv);
	}

	// This section just prints out the values to demonstrate that hash worked.
	if (help) {
		ARGS_PARSER::print_help();
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
		std::cout << "List of ints:\t" << list_of_ints_string.substr(0, list_of_ints_string.length() - 2) << std::endl;
		std::cout << "List of ints size:\t" << std::to_string(list_of_ints.size()) << std::endl;
		for (size_t i = 0; i < non_options.size(); i++) {
			std::cout << "NON_OPTION " << i << ":\t" << non_options[i] << std::endl;
		}
	}

	return 0;
}
