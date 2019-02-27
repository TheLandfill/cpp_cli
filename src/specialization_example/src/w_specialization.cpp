#include "parser.h"
#include "w_specialization.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char ** argv) {
	std::string filename = "a.out";
	int recursion_level = -1;
	std::string flag = "false";
	std::string flag2 = "false";
	double probability_of_success = 0.0001;
	std::string standard_input_hyphen = "";
	char verbosity[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	std::vector<const char *> non_options;

	bool w_sign_conversion = true;
	bool w_all = false;
	bool w_extra = false;
	int w_error_level = 1;
	int w_warning_level = 1;

	bool d_ignore_parentheses = false;
	bool d_allow_infinite_recursion = false;
	std::string d_configuration_name = "";
	int d_debug_level = 0;
	
	{
		Command_Line_Var<std::string> file_var(&filename, { "f", "file", "filename" }, true);
		Command_Line_Var<int> recursion_var(&recursion_level, { "r", "recursion", "max-depth" }, true);
		Command_Line_Var<std::string> flag_var(&flag, { "flag", "a" }, false);
		Command_Line_Var<std::string> flag2_var(&flag2, { "flag2", "b" }, false);
		Command_Line_Var<double> probability_of_success_var(&probability_of_success, { "p", "prob", "probability" }, true);
		Command_Line_Var<char> verbosity_var(verbosity, { "v" }, false, 20);
		Command_Line_Var<std::string> standard_input_hyphen_var(&standard_input_hyphen, { "-" }, false);
		Command_Line_Var<int> ignored_flag_var(nullptr, { "l", "library" }, true);

		// YOU DO NOT NEED TO USE W_SPECIALIZATIONS, W_VALUES, or W_ARGS
		W_SPECIALIZATION w_options(100);
		W_VALUE<bool> w_sign_conversion_var(&w_sign_conversion, w_options, "sign-conversion", true);
		W_VALUE<bool> w_no_sign_conversion_var(&w_sign_conversion, w_options, "no-sign-conversion" , false);
		W_VALUE<bool> w_all_var(&w_all, w_options, "all", true);
		W_VALUE<bool> w_extra_var(&w_extra, w_options, "extra", true);
		W_ARG<int> w_error_level_var(&w_error_level, w_options, "error-level");
		W_ARG<int> w_warning_level_var(&w_warning_level, w_options, "warning-level");
		Command_Line_Var<W_SPECIALIZATION> w_options_var(&w_options, { "W" }, true);

		W_SPECIALIZATION d_options(100);
		W_VALUE<bool> d_ignore_parentheses_var(&d_ignore_parentheses, d_options, "ignore-parentheses", true);
		W_VALUE<bool> d_allow_infinite_recursion_var(&d_allow_infinite_recursion, d_options, "allow-infinite-recursion", true);
		W_VALUE<bool> d_no_infinite_recursion_var(&d_allow_infinite_recursion, d_options, "no-infinite-recursion", false);
		W_ARG<std::string> d_configuration_name_var(&d_configuration_name, d_options, "config");
		W_ARG<int> d_debug_level_var(&d_debug_level, d_options, "level");
		Command_Line_Var<W_SPECIALIZATION> d_options_var(&d_options, { "D" }, true);

		non_options = ARGS_PARSER::parse(argc, argv);
	}

	std::cout << "filename:\t" << filename << std::endl;
	std::cout << "recursion:\t" << recursion_level << std::endl;
	std::cout << "flag:\t\t" << flag << std::endl;
	std::cout << "flag2:\t\t" << flag2 << std::endl;
	std::cout << "probability_of_success:\t" << probability_of_success << std::endl;
	std::cout << "standard_input_hyphen:\t" << standard_input_hyphen << std::endl;
	std::cout << "verbosity:\t" << verbosity << std::endl;

	std::cout << "w_sign_conversion:\t" << w_sign_conversion << std::endl;
	std::cout << "w_all:\t" << w_all << std::endl;
	std::cout << "w_extra:\t" << w_extra << std::endl;
	std::cout << "w_error_level:\t" << w_error_level << std::endl;
	std::cout << "w_warning_level:\t" << w_warning_level << std::endl;
	std::cout << "d_ignore_parentheses:\t" << d_ignore_parentheses << std::endl;
	std::cout << "d_allow_infinite_recursion:\t" << d_allow_infinite_recursion << std::endl;
	std::cout << "d_configuration_name:\t" << d_configuration_name << std::endl;
	std::cout << "d_debug_level:\t" << d_debug_level << std::endl;
	
	for (size_t i = 0; i < non_options.size(); i++) {
		std::cout << "NON_OPTION " << i << ":\t" << non_options[i] << std::endl;
	}
	return 0;
}
