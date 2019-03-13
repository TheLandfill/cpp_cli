#include "parser.h"
#include <string>
#include <iostream>

void push_prog(int argc, char ** argv, void * data);
void test_prog(int argc, char ** argv, void * data);
void pull_prog(int argc, char ** argv, void * data);

struct Main_Subcommand_Variables {
	std::string file_path;
	char verbosity[20];
	std::vector<const char *> non_options;
	Main_Subcommand_Variables() : verbosity("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") {}
};

int main(int argc, char ** argv) {
	Main_Subcommand_Variables msv;
	Command_Line_Var<std::string> file_path_var(msv.file_path, { "path", "p" }, true);
	Command_Line_Var<char> verbosity_var(msv.verbosity, { "v" }, false, 20);
	
	ARGS_PARSER::add_subcommand("pull", pull_prog);
	ARGS_PARSER::add_subcommand("push", push_prog);

	msv.non_options = ARGS_PARSER::parse(argc, argv, &msv);
	std::cout << "file_path: " << msv.file_path << std::endl;
	std::cout << "verbosity: " << msv.verbosity << std::endl;
	
	std::string nesting = "";
	for (size_t i = 0; i < msv.non_options.size(); i++) {
		if (msv.non_options[i] == nullptr) {
			nesting += "\t";
			continue;
		}
		std::cout << nesting << "NON_OPTION " << std::to_string(i) << ": " << msv.non_options[i] << std::endl;
	}
}

void push_prog(int argc, char ** argv, void * data) {
	Main_Subcommand_Variables * fixed_data = (Main_Subcommand_Variables*)data;
	std::cout << std::endl << "IN SUBCOMMAND PUSH" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;
	std::cout << "file_path: " << fixed_data->file_path << std::endl;
	std::cout << "verbosity: " << fixed_data->verbosity << std::endl;
	std::string URL = "";
	Command_Line_Var<std::string> URL_var(URL, { "u", "URL" }, true);

	ARGS_PARSER::add_subcommand("test", test_prog);
	try {
		ARGS_PARSER::parse(argc, argv, &URL);
	} catch (std::invalid_argument& e) {
		std::cerr << "INVALID ARGUMENT FOR SUBCOMMAND PUSH" << std::endl;
		throw e;
	}

	std::cout << "URL: " << URL << std::endl;
	std::cout << "LEAVING SUBCOMMAND PUSH" << std::endl << std::endl;
}

void pull_prog(int argc, char ** argv, void * data) {
	(void)data;
	std::cout << std::endl << "IN SUBCOMMAND PULL" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;
	std::string URL = "";
	unsigned long long timeout = 100;
	Command_Line_Var<std::string> URL_var(URL, { "u", "URL" }, true);
	Command_Line_Var<unsigned long long> timeout_var(timeout, { "t", "timeout" }, true);
	try {
		ARGS_PARSER::parse(argc, argv, nullptr);
	} catch (std::invalid_argument& e) {
		std::cerr << "INVALID ARGUMENT FOR SUBCOMMAND PULL" << std::endl;
		throw e;
	}
	std::cout << "URL: " << URL << std::endl;
	std::cout << "Timeout: " << timeout << std::endl;
	std::cout << "LEAVING SUBCOMMAND PULL" << std::endl << std::endl;
}

void test_prog(int argc, char ** argv, void * data) {
	std::cout << std::endl << "IN SUBCOMMAND TEST" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;
	std::cout << "URL: " << *(std::string *)data << std::endl;

	int underwear_count = 0;
	double EURL = -1;

	Command_Line_Var<int> underwear_count_var(underwear_count, { "u" }, true);
	Command_Line_Var<double> EURL_var(EURL, { "e", "E", "EURL", "URL" }, true);

	ARGS_PARSER::parse(argc, argv, nullptr);

	std::cout << "underwear_count: " << underwear_count << std::endl;
	std::cout << "EURL: " << EURL << std::endl;
}
