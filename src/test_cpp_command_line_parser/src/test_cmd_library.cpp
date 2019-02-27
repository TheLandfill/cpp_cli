#include "parser.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char ** argv) {

	// sample variables that you want to initialize with default values set just
	// like you would in any program

	std::string filename = "a.out";
	int recursion_level = -1;
	std::string flag = "false";
	std::string flag2 = "false";
	double probability_of_success = 0.0001;
	std::string standard_input_hyphen = "";
	char verbosity[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

	// non_options is a vector of args that did not start with a hyphen, did not
	// start with a double hyphen, or came after the special argument "--".
	// non_options orders its elements by the same order they came in on the
	// command line.
	std::vector<const char *> non_options;

	// This scoping is just to automatically delete the Command_Line_Vars after
	// they are no longer necessary. They need to be in accessable in the same
	// scope as when hash is called.
	{

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
		// arguments in the forms -aValue or -a Value, but a short option's
		// argument cannot start with a dash if it is separated by a space. The
		// syntax -a-Negative_Value is acceptable, but -a -Negative_Value is not.
		// -Negative_Value will be interpreted as a flag in the second case, and
		// potentially cause an error as the library will interpret it as either
		// -N egative_Value or -N -e -g -a, etc. depending on whether every
		// character is a valid flag or not.

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

		Command_Line_Var<std::string> file_var(&filename, { "f", "file", "filename" }, true);
		Command_Line_Var<int> recursion_var(&recursion_level, { "r", "recursion", "max-depth" }, true);
		Command_Line_Var<std::string> flag_var(&flag, { "flag", "a" }, false);
		Command_Line_Var<std::string> flag2_var(&flag2, { "flag2", "b" }, false);
		Command_Line_Var<double> probability_of_success_var(&probability_of_success, { "p", "prob", "probability" }, true);
		Command_Line_Var<char> verbosity_var(verbosity, { "v" }, false, 20);

		// Generally, the solitary hyphen flag is used to indicate that the
		// program should take in standard input. The library will tell you that
		// someone typed a hyphen, and it is up to you to determine what you
		// want to do with it. It is not necessary to do anything with it. If
		// Unless you do something with it, the hyphen will be ignored.
		Command_Line_Var<std::string> standard_input_hyphen_var(&standard_input_hyphen, { "-" }, false);

		// This will put any argument starting with "-l" or "--library" into
		// non_options, even though it's still an option, as its order matters.
		// The type doesn't matter as you're not setting a variable. You will
		// have to take template specializations into account, such as char.
		// You should also set takes_args to be true, or else its position does
		// not matter.
		Command_Line_Var<int> ignored_flag_var(nullptr, { "l", "library" }, true);

		// Having the same flag refer to two different Command_Line_Vars will
		// cause the program to fail and tell you which flag.
		// Command_Line_Var<int> breaks_program(nullptr, { "l" }, false);

		// Non options must be declared outside the scope unless you don't
		// need to use them.
		non_options = ARGS_PARSER::parse(argc, argv);
	}

	// This section just prints out the values to demonstrate that hash worked.
	std::cout << "filename:\t" << filename << std::endl;
	std::cout << "recursion:\t" << recursion_level << std::endl;
	std::cout << "flag:\t\t" << flag << std::endl;
	std::cout << "flag2:\t\t" << flag2 << std::endl;
	std::cout << "probability_of_success:\t" << probability_of_success << std::endl;
	std::cout << "standard_input_hyphen:\t" << standard_input_hyphen << std::endl;
	std::cout << "verbosity:\t" << verbosity << std::endl;
	for (size_t i = 0; i < non_options.size(); i++) {
		std::cout << "NON_OPTION " << i << ":\t" << non_options[i] << std::endl;
	}

	return 0;
}
