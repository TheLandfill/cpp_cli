#include "cpp_cli.h"
#include "w_specialization.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char ** argv) {
	using namespace cli;
	std::string filename = "a.out";
	int recursion_level = -1;
	std::string flag = "false";
	std::string flag2 = "false";
	double probability_of_success = 0.0001;
	bool standard_input_hyphen = false;
	size_t verbosity = 0;
	std::vector<const char *> non_options;

	bool w_sign_conversion = true;
	bool w_all = false;
	bool w_extra = false;
	int w_error_level = 1;
	int w_warning_level = 1;
	char w_type = 'x';

	bool d_ignore_parentheses = false;
	bool d_allow_infinite_recursion = false;
	std::string d_configuration_name = "";
	int d_debug_level = 0;

	bool help = false;
	
	{
		Parser p;
		p.arg(filename, { "f", "file", "filename" }, "Dummy variable corresponding to some sort of file name, usually either and input or an output file.");
		p.arg(recursion_level, { "r", "recursion", "max-depth" }, "Dummy variable corresponding to some sort of recursion restriction.");
		p.arg(probability_of_success, { "p", "prob", "probability" }, "Determines the probability that this program will work.");
		p.repeated(verbosity, { "v" }, "Dummy variable that corresponds to an option to set the verbosity.");
		p.value(standard_input_hyphen, { "-" }, true, "Just here to show that the standard input hyphen still works with all the W_specialization stuff.");
		p.ignored({ "l", "library" }, "I copied these from an earlier test program.");

		// YOU DO NOT NEED TO USE WSpecializationS, WvalueS, or WargS
		WSpecialization w_options(100);
		Wvalue<bool> w_sign_conversion_var(w_sign_conversion, w_options, "sign-conversion", true);
		Wvalue<bool> w_no_sign_conversion_var(w_sign_conversion, w_options, "no-sign-conversion" , false);
		Wvalue<bool> w_all_var(w_all, w_options, "all", true);
		Wvalue<bool> w_extra_var(w_extra, w_options, "extra", true);
		Wvalue<char> w_type_a_var(w_type, w_options, "file", 'f');
		Wvalue<char> w_type_b_var(w_type, w_options, "dir", 'd');
		Wvalue<char> w_type_c_var(w_type, w_options, "link", 'l');
		Wvalue<char> w_type_d_var(w_type, w_options, "any", 'a');
		Warg<int> w_error_level_var(w_error_level, w_options, "error-level");
		Warg<int> w_warning_level_var(w_warning_level, w_options, "warning-level");
		p.arg(w_options, { "W" }, "Only the -W main argument has a help message at this point because these options are better documented outside of a simple help message.");

		WSpecialization d_options(100);
		Wvalue<bool> d_ignore_parentheses_var(d_ignore_parentheses, d_options, "ignore-parentheses", true);
		Wvalue<bool> d_allow_infinite_recursion_var(d_allow_infinite_recursion, d_options, "allow-infinite-recursion", true);
		Wvalue<bool> d_no_infinite_recursion_var(d_allow_infinite_recursion, d_options, "no-infinite-recursion", false);
		Warg<std::string> d_configuration_name_var(d_configuration_name, d_options, "config");
		Warg<int> d_debug_level_var(d_debug_level, d_options, "level");
		p.arg(d_options, { "D" }, "Just here for fun.");

		p.value(help, { "help", "h" }, true, "Displays this help message and exits.");

		const char * header = "This program will basically show you that this library can emulate gcc's -W flag for multiple variables."
		" While I'm here, I might as well show you that your compiler will concatenate string literals if you put them in sequence, "
		"which should make them easier to read, especially if you plan on writing a big long description like what I'm writing right now. The library will also make "
		"sure all this text is formatted properly within whatever help width you specify. The default is 80 characters, but I've set it to 60 to prove that you can.";

		const char * usage = "[options/non-options]\n\n"
		"Once again, these test programs don't really have any superstructure to the options/non-options, so I can't really be any more specific.";

		const char * footer = "For more information, contact me at the.landfill.coding@gmail.com or on the github page. You could also put your version information"
		" stuff here, which would be cool.";
		
		p.set_header(header);
		p.set_usage(usage);
		p.set_footer(footer);
		p.set_help_width(90);
		p.set_help_file_path("");

		p.generate_help(argv[0]);

		non_options = p.parse(argc, argv);

		if (help) {
			p.print_help();
			return 0;
		}
	}

	std::cout << "--------------------STANDARD--------------------" << std::endl;
	std::cout << "filename:\t" << filename << std::endl;
	std::cout << "recursion:\t" << recursion_level << std::endl;
	std::cout << "flag:\t\t" << flag << std::endl;
	std::cout << "flag2:\t\t" << flag2 << std::endl;
	std::cout << "probability_of_success:\t" << probability_of_success << std::endl;
	std::cout << "standard_input_hyphen:\t" << standard_input_hyphen << std::endl;
	std::cout << "verbosity:\t" << verbosity << std::endl;

	std::cout << "--------------------W FLAGS---------------------" << std::endl;
	std::cout << "w_sign_conversion:\t" << w_sign_conversion << std::endl;
	std::cout << "w_all:\t" << w_all << std::endl;
	std::cout << "w_extra:\t" << w_extra << std::endl;
	std::cout << "w_error_level:\t" << w_error_level << std::endl;
	std::cout << "w_warning_level:\t" << w_warning_level << std::endl;
	std::cout << "w_type:\t" << w_type << std::endl;

	std::cout << "--------------------D FLAGS---------------------" << std::endl;
	std::cout << "d_ignore_parentheses:\t" << d_ignore_parentheses << std::endl;
	std::cout << "d_allow_infinite_recursion:\t" << d_allow_infinite_recursion << std::endl;
	std::cout << "d_configuration_name:\t" << d_configuration_name << std::endl;
	std::cout << "d_debug_level:\t" << d_debug_level << std::endl;
	
	for (size_t i = 0; i < non_options.size(); i++) {
		std::cout << "NON_OPTION " << i << ":\t" << non_options[i] << std::endl;
	}
	return 0;
}
