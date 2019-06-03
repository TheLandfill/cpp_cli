#include "cpp_cli.h"
#include <string>
#include <iostream>

void push_prog(int argc, char ** argv, void * data);
void test_prog(int argc, char ** argv, void * data);
void pull_prog(int argc, char ** argv, void * data);

bool help = false;

struct Main_Subcommand_Variables {
	std::string file_path;
	size_t verbosity = 0;
	std::vector<const char *> non_options;
};

int main(int argc, char ** argv) {
	using namespace cli;
	Parser p;
	Main_Subcommand_Variables msv;
	p.arg(msv.file_path, { "path", "p" }, "Dummy variable.");
	p.repeated(msv.verbosity, { "v" }, "Dummy variable.");
	p.value(help, { "h", "help" }, true, "Prints this help message and exits.");
	
	p.add_subcommand("pull", pull_prog, "Does something like 'git pull' if this program actually did anything.");
	p.add_subcommand("push", push_prog, "Does something like 'git push' if this program actually did anything.");

	p.set_help_file_path("");

	p.set_usage("[options/non-options] [subcommand]\n\t\t[subcommand's options/non-options] [subcommand's subcommand]\n\n"
	"Since this program kind of uses recursion when dealing with subcommands, the general pattern of [command] [command's options/non-options] [subcommand] is"
	" repeated, which each subcommand acting like a completely new command with its own options and subcommands.");

	p.set_header("This program serves mainly to demonstrate how subcommands work in the library. "
	"Essentially, each subcommand acts just like you wrote the program for the subcommand completely on its own without any "
	"knowledge of any supercommands or sibling commands except what information their supercommand passes to them, global variables, and the "
	"header, usage, and footers of their supercommands unless you change them. Changing them will have no effect on the help message of the "
	"supercommand, since you can only generate the help message before running 'Parser::parse'. "
	"In this case, 'push' and 'pull' are sibling commands, with both being subcommands to the main program, and 'push' has its own subcommand 'test'. "
	"Furthermore, they all have their own flags completely independent from any other subcommands.");
	
	p.set_footer("For more information, contact me at the.landfill.coding@gmail.com or on the github page. You could also put your version information stuff here, which would be cool.");

	p.generate_help(argv[0]);

	msv.non_options = p.parse(argc, argv, &msv);

	if (help) {
		p.print_help();
		return 0;
	}

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
	std::cout << "argc for push: " << argc << "\n";
	for (int i = 0; i < argc; i++) {
		std::cout << i << "\t" << argv[i] << "\n";
	}
	using namespace cli;
	Parser p;
	Main_Subcommand_Variables * fixed_data = static_cast<Main_Subcommand_Variables*>(data);
	std::string URL = "";
	p.arg(URL, { "u", "URL" }, "Sets the URL.");
	p.value(help, { "h", "help" }, true, "Displays this help message and exits.");

	p.add_subcommand("test", test_prog);
	p.generate_help(argv[0]);
	try {
		p.parse(argc, argv, &URL);
	} catch (std::invalid_argument& e) {
		std::cerr << "INVALID ARGUMENT FOR SUBCOMMAND PUSH" << std::endl;
		throw;
	}

	if (help) {
		return;
	}
	std::cout << std::endl << "IN SUBCOMMAND PUSH" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;
	std::cout << "file_path: " << fixed_data->file_path << std::endl;
	std::cout << "verbosity: " << fixed_data->verbosity << std::endl;


	std::cout << "URL: " << URL << std::endl;
	std::cout << "LEAVING SUBCOMMAND PUSH" << std::endl << std::endl;
}

void pull_prog(int argc, char ** argv, void * data) {
	std::cout << "argc for pull: " << argc << "\n";
	for (int i = 0; i < argc; i++) {
		std::cout << i << "\t" << argv[i] << "\n";
	}
	using namespace cli;
	Parser p;
	(void)data;
	std::string URL = "";
	unsigned long long timeout = 100;
	p.arg(URL, { "u", "URL" }, "Sets the URL.");
	p.arg(timeout, { "t", "timeout" }, "Sets the amount of time before a timeout.");
	p.value(help, { "h", "help" }, true, "Displays this help message and exits.");
	p.generate_help(argv[0]);
	try {
		p.parse(argc, argv, nullptr);
	} catch (std::invalid_argument& e) {
		std::cerr << "INVALID ARGUMENT FOR SUBCOMMAND PULL" << std::endl;
		throw;
	}
	
	if (help) {
		return;
	}
	std::cout << std::endl << "IN SUBCOMMAND PULL" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;


	std::cout << "URL: " << URL << std::endl;
	std::cout << "Timeout: " << timeout << std::endl;
	std::cout << "LEAVING SUBCOMMAND PULL" << std::endl << std::endl;
}

void test_prog(int argc, char ** argv, void * data) {
	std::cout << "argc for test: " << argc << "\n";
	for (int i = 0; i < argc; i++) {
		std::cout << i << "\t" << argv[i] << "\n";
	}
	using namespace cli;
	Parser p;
	int underwear_count = 0;
	double EURL = -1;

	p.arg(underwear_count, { "u" }, "Sets the number of underwear currently available. Note that this has no idea that '-u' corresponded to the URL variable in the push subcommand.");
	p.arg(EURL, { "e", "E", "EURL", "URL" }, "Sets the value of EURL. Note that '--URL' and '-u' correspond to completely different variables.");
	p.value(help, { "h", "help" }, true, "Displays this help message and exits.");

	p.set_usage("\n\tTest.");

	p.set_header("I'm just changing this to show you that changing any of these help message variables does nothing to any other help message.");

	p.generate_help(argv[0]);
	p.parse(argc, argv, nullptr);

	if (help) {
		return;
	}

	std::cout << std::endl << "IN SUBCOMMAND TEST" << std::endl;
	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv[0]: " << argv[0] << std::endl;
	std::cout << "URL: " << *(std::string *)data << std::endl;

	std::cout << "underwear_count: " << underwear_count << std::endl;
	std::cout << "EURL: " << EURL << std::endl;
}
