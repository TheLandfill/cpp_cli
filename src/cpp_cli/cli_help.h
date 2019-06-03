#ifndef CLI_HELP_H
#define CLI_HELP_H
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class CLI_Interface;

namespace cli {
struct CLI_Help {
public:
	std::string header;
	std::string usage;
	std::string footer;
	static std::string help_file_name;
	static std::string help_file_path;
	size_t help_width = 80;

	static std::vector<const char *> current_command_list;
	std::vector<const char *> subcommand_descriptions;
public:
	void set_usage(std::string u);
	void set_header(std::string h);
	void set_footer(std::string f);
	void set_help_width(size_t hw);
	void set_help_file_name(std::string hfn);
	void set_help_file_path(std::string hfp);

	void print_within_length_stream(const std::string& str, size_t indent = 0, std::ostream& file_writer = std::cout);
	std::string print_within_length_str(const std::string& str, size_t indent = 0);

	void generate_help(const char * subcommand_name, std::vector<const char *> subcommand_aliases, std::vector<CLI_Interface *> list_of_cmd_var);
	void print_help();
	
};

inline void CLI_Help::set_header(std::string h) {
	header = h;
}

inline void CLI_Help::set_usage(std::string u) {
	usage = u;
}

inline void CLI_Help::set_footer(std::string f) {
	footer = f;
}

inline void CLI_Help::set_help_width(size_t hw) {
	help_width = hw;
}

inline void CLI_Help::set_help_file_name(std::string hfn) {
	help_file_name = hfn;
}

inline void CLI_Help::set_help_file_path(std::string hfp) {
	help_file_path = hfp;
}

inline void CLI_Help::generate_help(const char * subcommand_name, std::vector<const char *> subcommand_aliases, std::vector<CLI_Interface *> list_of_cmd_var) {
	if (subcommand_name[0] == '.' && subcommand_name[1] == '/') {
		subcommand_name += 2;
	}
	current_command_list.push_back(subcommand_name);
	std::string buffer;
	buffer.reserve(2048);
	if (help_file_path == "`") {
		std::string error_message = "The help file path has not been set. "
		"Use the command 'CLI_Help::set_help_file_path(std::string hfn)' to set a valid file path before calling generate_help. "
		"The file path should be an absolute path if you want the program to run anywhere. "
		"Only use a relative path if your executable can only be executed from one spot.";
		throw std::runtime_error(error_message);
	}
	buffer = help_file_path;
	buffer += ".";
	for (size_t i = 0; i < current_command_list.size(); i++) {
		buffer += current_command_list[i];
		buffer += "_";
	}
	buffer += "help_file";
	set_help_file_name(buffer);

	std::ofstream file_writer;
	file_writer.open(help_file_name);

	if (!file_writer.is_open()) {
		std::string error_message;
		error_message.reserve(1024);
		error_message += help_file_name;
		error_message += " must be accessable by the current user.";
		throw std::runtime_error(error_message);
	}

	buffer = "usage: ";
	for (size_t i = 0; i < current_command_list.size(); i++) {
		buffer += current_command_list[i];
		buffer += " ";
	}

	buffer += usage;

	print_within_length_stream(buffer, 0, file_writer);
	file_writer << "\n";
	print_within_length_stream(header, 0, file_writer);
	file_writer << "\n";

	bool any_descriptions = false;

	for (size_t i = 0; !any_descriptions && i < subcommand_descriptions.size(); i++) {
		any_descriptions = subcommand_descriptions[i][0] != '`';
	}

	if (any_descriptions) {
		file_writer << "SUBCOMMANDS:\n";
		for (size_t i = 0; i < subcommand_descriptions.size(); i++) {
			if (subcommand_descriptions[i][0] != '`') {
				file_writer << subcommand_aliases[i] << "\n";
				print_within_length_stream(subcommand_descriptions[i], 8, file_writer);
			}
		}
		file_writer << "\n";
	}

	file_writer << "OPTIONS:\n";
	for (size_t i = 0; i < list_of_cmd_var.size(); i++) {
		CLI_Interface * clv = list_of_cmd_var[i];
		const std::vector<const char *>& a = clv->get_aliases();
		buffer = "";
		if (clv->get_help_message()[0] != '`') {
			const char * n_dash = "--";
			
			for (size_t j = 0; j < a.size(); j++) {
				buffer += n_dash + (a[j][1] == '\0') * (1 + (a[j][0] == '-'));
				buffer += a[j];
				buffer += ", ";
			}
			buffer.pop_back();
			buffer.pop_back();

			file_writer << buffer << "\n";
			print_within_length_stream(std::string(clv->get_help_message()), 8, file_writer);
		}
	}	
	file_writer << "\n";
	print_within_length_stream(footer, 0, file_writer);
	file_writer.close();
}

inline void CLI_Help::print_help() {
	std::ifstream file_reader;
	file_reader.open(help_file_name);
	if (!file_reader.is_open()) {
		std::string error_message;
		error_message.reserve(1024);
		error_message += "\"";
		error_message += help_file_name;
		error_message += "\" in \"";
		error_message += help_file_path;
		error_message += "\"";
		if (help_file_path == "") {
			error_message += " (current running directory)";
		}
		error_message += " has not been generated. Please put CLI_Help::generate_help(argv[0]) right before calling parse in the subcommand: ";
		for (size_t i = 0; i < error_message.size(); i++) {
			error_message += current_command_list[i];
			error_message += "->";
		}
		error_message.pop_back();
		error_message.pop_back();
		throw std::runtime_error(error_message);
	} else {
		std::string line;
		while (std::getline(file_reader, line)) {
			std::cout << line << "\n";
		}
	}
	file_reader.close();
}

void CLI_Help::print_within_length_stream(const std::string& str, size_t indent, std::ostream& stream) {
	size_t cur_index = 0;
	std::string indent_str(indent, ' ');
	while (cur_index + help_width < str.length()) {
		size_t line_length = str.find_last_of(" \t", cur_index + help_width - indent);
		size_t next_newline = str.find("\n", cur_index);
		if (next_newline < line_length) {
			line_length = next_newline;
		}
		stream << indent_str << str.substr(cur_index, line_length - cur_index) << "\n";
		cur_index = line_length + 1;
	}
	stream << std::string(indent, ' ') << str.substr(cur_index) << "\n";
}

std::string CLI_Help::print_within_length_str(const std::string& str, size_t indent) {
	size_t cur_index = 0;
	std::string next_str;
	next_str.reserve(str.length());
	while (cur_index < str.length()) {
		size_t line_length = str.rfind(" ", cur_index + help_width - indent);
		next_str += std::string(indent, ' ');
		next_str += str.substr(cur_index, line_length - cur_index);
		next_str += "\n";
		cur_index += line_length + 1;
	}
	return next_str;
}

std::string CLI_Help::help_file_path = "`";
std::string CLI_Help::help_file_name = "If you see this message, the help_file_name is not being set.";
std::vector<const char *> CLI_Help::current_command_list = std::vector<const char *>();
}

#endif
