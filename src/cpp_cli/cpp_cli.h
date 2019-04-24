#ifndef CPP_CMD_LINE_PARSER_H
#define CPP_CMD_LINE_PARSER_H
#include "args_parser_templates.h"
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <cstdio>

namespace cli{
	
class Parser {
friend class CLI_Interface;
public:
	typedef void (*subcommand_func)(int, char **, void *);
private:
	static std::unordered_map<std::string, CLI_Interface *> command_line_settings_map;
	static std::vector<CLI_Interface *> list_of_cmd_var;
	static std::vector<const char *> non_options;

	static std::unordered_map<std::string, subcommand_func> subcommand_map;
	static std::vector<subcommand_func> subcommand_list;
	static std::vector<const char *> subcommand_aliases;
	static std::vector<const char *> subcommand_descriptions;

	static size_t num_unique_flags;

	static std::string header;
	static std::string usage;
	static std::string footer;
	static std::string help_file_name;
	static std::string help_file_path;
	static size_t help_width;
	static std::vector<const char *> current_command_list;
private:
	static void fill_hash_table();
	static void fill_subcommand_hash_table();

	static void subcommand_handling(int argc, char ** argv, void * data);

	static void long_option_handling(char ** argv, int& i);
	static int find_and_mark_split_location(char * flag);
	static void check_if_option_exists(const char * error_message, const char * flag, const bool exists, const bool should_exist);

	static void short_option_handling(int argc, char ** argv, int& i);
	static void multiple_short_options_handling(int argc, char ** argv, int& cur_argument);

	static void clear_everything();
	static void clear_memory();

	static void print_flags();
public:
	static void print_within_length_c_file(const char * str, size_t indent = 0, FILE * file_writer = stdout);
	static int print_within_length_c_str(const char * str, char * buffer, int bf_size, size_t indent = 0);

	static void print_within_length_stream(const std::string& str, size_t indent = 0, std::ostream& file_writer = std::cout);
	static std::string print_within_length_str(const std::string& str, size_t indent = 0);
	
	static void add_subcommand(const char * subcommand, subcommand_func sub_func, const char * description = "");
	static void reserve_space_for_subcommand(size_t number_of_subcommand);

	static void set_usage(std::string u);
	static void set_header(std::string h);
	static void set_footer(std::string f);
	static void set_help_width(size_t hw);
	static void set_help_file_name(std::string hfn);
	static void set_help_file_path(std::string hfp);

	static void generate_help(const char * command_name);
	static void print_help();
	
	static std::vector<const char *> parse(int argc, char ** argv, void * data = nullptr);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////INLINE DECLARATIONS//////////////////////////////
///////////////////////////////////////////////////////////////////////////////

inline std::vector<const char *> Parser::parse(int argc, char ** argv, void * data) {
	fill_hash_table();
	fill_subcommand_hash_table();
	non_options.reserve(2 * argc);
	int i = 1;
	for (; i < argc; i++) {
		// case: subcommand, which is recursive
		if (subcommand_map.count(argv[i]) != 0) {
			subcommand_handling(argc - i, argv + i, data);
			break;
		}
		// cases: --long-option
		else if (argv[i][2] != '\0' && argv[i][1] == '-' && argv[i][0] == '-') {
			long_option_handling(argv, i);
		// case: -- and all arguments are options
		} else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == '\0') {
			i++;
			for (; i < argc; i++) {
				non_options.push_back(argv[i]);
			}
			break;
		// case: -
		} else if (argv[i][0] == '-' && argv[i][1] == '\0' && command_line_settings_map.count("-") != 0) {
			command_line_settings_map["-"]->set_base_variable("-");
		} else if (argv[i][0] == '-') {
			short_option_handling(argc, argv, i);
		} else {
			non_options.push_back(argv[i]);
		}
	}
	clear_memory();
	return non_options;
}

inline void Parser::set_header(std::string h) {
	header = h;
}

inline void Parser::set_usage(std::string u) {
	usage = u;
}

inline void Parser::set_footer(std::string f) {
	footer = f;
}

inline void Parser::set_help_width(size_t hw) {
	help_width = hw;
}

inline void Parser::set_help_file_name(std::string hfn) {
	help_file_name = hfn;
}

inline void Parser::set_help_file_path(std::string hfp) {
	help_file_path = hfp;
}

inline void Parser::generate_help(const char * subcommand_name) {
	if (subcommand_name[0] == '.' && subcommand_name[1] == '/') {
		subcommand_name += 2;
	}
	current_command_list.push_back(subcommand_name);
	std::string buffer;
	buffer.reserve(2048);
	if (help_file_path == "`") {
		std::string error_message = print_within_length_str(std::string("The help file path has not been set. "
		"Use the command 'Parser::set_help_file_path(std::string hfn)' to set a valid file path before calling generate_help. "
		"The file path should be an absolute path if you want the program to run anywhere. "
		"Only use a relative path if your executable can only be executed from one spot."));
		throw std::runtime_error(buffer);
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

	buffer = "usage:";
	for (size_t i = 0; i < current_command_list.size(); i++) {
		buffer += " ";
		buffer += current_command_list[i];
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

inline void Parser::print_help() {
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
		error_message += " has not been generated. Please put Parser::generate_help(argv[0]) right before calling parse in the subcommand: ";
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

inline int Parser::print_within_length_c_str(const char * str, char * buffer, int bf_size, size_t indent) {
	size_t i = 0;
	int last_position = -1;
	const char * initial_str = str;
	while (str[i] != '\0' && (size_t)(str - initial_str) < bf_size - help_width - 1) {
		while (str[i] != '\0' && str[i] != ' ' && str[i] != '\n' && str[i] != '\t') {
			i++;
		}
		while (str[i] != '\0' && i < help_width - indent) {
			int line_break = -(str[i] == ' ' || str[i] == '\t' || str[i] == '\n');
			last_position = (i & line_break) | (last_position & ~line_break);
			if (str[i] == '\n') {
				break;
			}
			i++;
		}

		if (str[i] == '\0') {
			buffer += sprintf(buffer, "%*s\n", (int)(indent + i), str);
			str += i;
		} else {
			buffer += sprintf(buffer, "%*.*s\n", last_position + (int)indent, last_position, str);
			str += last_position + 1;
		}
		i = 0;
	}
	return str - initial_str;
}

inline void Parser::print_within_length_c_file(const char * str, size_t indent, FILE * file_writer) {
	const int bf_size = 2048;
	char buffer[bf_size];
	const char * i_str = str;
	while (str[0] != '\0') {
		str += print_within_length_c_str(str, buffer, bf_size, indent);
		fprintf(file_writer, "%s", buffer);
	}
	if (i_str[0] != '\0') {
		fprintf(file_writer, "\n");
	}
}

void Parser::print_within_length_stream(const std::string& str, size_t indent, std::ostream& stream) {
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

std::string Parser::print_within_length_str(const std::string& str, size_t indent) {
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

inline void Parser::fill_hash_table() {
	command_line_settings_map = std::unordered_map<std::string, CLI_Interface *>(num_unique_flags);
	std::unordered_map<std::string, int *> flag_already_used(num_unique_flags);
	for (size_t i = 0; i < Parser::list_of_cmd_var.size(); i++) {
		CLI_Interface * cur_com_var = list_of_cmd_var[i];
		const std::vector<const char *> & cur_aliases = cur_com_var->get_aliases();
		for (size_t j = 0; j < cur_aliases.size(); j++) {
			check_if_option_exists("Flag already used: ", cur_aliases[j], flag_already_used.count(cur_aliases[j]), false);
			flag_already_used.insert({cur_aliases[j], nullptr});
			command_line_settings_map.insert({cur_aliases[j], cur_com_var});
		}
	}
}

inline void Parser::subcommand_handling(int argc, char ** argv, void * data) {
	subcommand_func sub_com = *(subcommand_map[argv[0]]);
	clear_everything();
	non_options.push_back(nullptr);
	non_options.push_back(argv[0]);
	sub_com(argc, argv, data);
}

inline void Parser::reserve_space_for_subcommand(size_t number_of_subcommand) {
	subcommand_list.reserve(number_of_subcommand);
	subcommand_aliases.reserve(number_of_subcommand);
}

inline void Parser::add_subcommand(const char * subcommand, Parser::subcommand_func sub_func, const char * description) {
	subcommand_list.push_back(sub_func);
	subcommand_aliases.push_back(subcommand);
	subcommand_descriptions.push_back(description);
}

inline void Parser::fill_subcommand_hash_table() {
	size_t n_sub = subcommand_aliases.size();
	subcommand_map.reserve(2 * n_sub);
	std::unordered_map<std::string, int *> flag_already_used(2 * n_sub);
	for (size_t i = 0; i < n_sub; i++) {
		check_if_option_exists("Subcommand already used: ", subcommand_aliases[i], flag_already_used.count(subcommand_aliases[i]), false);
		flag_already_used.insert({subcommand_aliases[i], nullptr});
		subcommand_map.insert({subcommand_aliases[i], subcommand_list[i]});
	}
}

inline void Parser::clear_everything() {
	subcommand_map.clear();
	subcommand_list.clear();
	subcommand_aliases.clear();
	subcommand_descriptions.clear();
	list_of_cmd_var.clear();
	command_line_settings_map.clear();
	num_unique_flags = 0;
	// non_options is never deleted.
}

inline void Parser::clear_memory() {
	subcommand_map.clear();
	command_line_settings_map.clear();

	std::vector<subcommand_func>().swap(subcommand_list);
	std::vector<const char *>().swap(subcommand_aliases);
	std::vector<CLI_Interface *>().swap(list_of_cmd_var);
	std::vector<const char *>().swap(subcommand_descriptions);
	num_unique_flags = 0;
}

inline int Parser::find_and_mark_split_location(char * flag) {
	int split_location = 0;
	for (; flag[split_location] != '\0'; split_location++) {
		if (flag[split_location] == '=') {
			flag[split_location] = '\0';
			split_location++;
			break;
		}
	}
	return split_location;
}

inline void Parser::check_if_option_exists(const char * error_message, const char * potential_option, const bool exists, const bool should_exist) {
	if (exists != should_exist) {
		std::string error_message_buffer;
		error_message_buffer.reserve(1024);
		error_message_buffer += error_message;
		error_message_buffer += potential_option;
		throw std::invalid_argument(error_message_buffer);
	}
}

inline void Parser::long_option_handling(char ** argv, int& i) {
	char * temp_alias = argv[i] + 2;
	int split_location = find_and_mark_split_location(temp_alias);

	check_if_option_exists("Unrecognized Option: --", temp_alias, command_line_settings_map.count(temp_alias), true);

	if (command_line_settings_map[temp_alias]->ignored()) {
		if (command_line_settings_map[temp_alias]->takes_args()) {
			temp_alias[split_location - 1] = '=';
		}
		non_options.push_back(argv[i]);
		return;
	}

	// case: --long-option=value
	if (temp_alias[split_location] != '\0') {
		if (command_line_settings_map[temp_alias]->takes_args()) {
			command_line_settings_map[temp_alias]->set_base_variable(temp_alias + split_location);
			temp_alias[split_location - 1] = '=';
		} else {
			std::string error_message;
			error_message.reserve(1024);
			error_message += "Option does not take arguments: --";
			error_message += temp_alias;
			throw std::invalid_argument(error_message);
		}
	// case: --long-option
	} else if (command_line_settings_map[temp_alias]->takes_args()) {
		std::string error_message;
		error_message.reserve(1024);
		error_message += "Option requires arguments: --";
		error_message += temp_alias;
		throw std::invalid_argument(error_message);
	} else {
		command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
	}
}

inline void Parser::short_option_handling(int argc, char ** argv, int& i) {
	char temp_alias[2] = "\0";
	temp_alias[0] = argv[i][1];

	check_if_option_exists("Unrecognized Option: -", temp_alias, command_line_settings_map.count(temp_alias), true);

	if (command_line_settings_map[temp_alias]->ignored()) {
		non_options.push_back(argv[i]);
		return;
	}

	// case: -o value // -o already exists because we checked for it initally.
	if (argv[i][2] == '\0' && i + 1 < argc && command_line_settings_map[argv[i] + 1]->takes_args()) {
		command_line_settings_map[argv[i] + 1]->set_base_variable(argv[i + 1]);
		i++;
		return;
	}

	// case: -oValue
	if (command_line_settings_map[temp_alias]->takes_args()) {
		command_line_settings_map[temp_alias]->set_base_variable(argv[i] + 2);
		return;
	}

	// case: -abc or -vvv
	multiple_short_options_handling(argc, argv, i);
}

inline void Parser::multiple_short_options_handling(int argc, char ** argv, int& cur_argument) {
	int i = 0;
	char * flag = argv[cur_argument] + 1;
	char temp_alias[2] = "\0";

	// case -vvv
	std::string temp_repetition;
	temp_repetition.reserve(32);
	bool repeated_short_arguments = false;
	while (flag[i] != '\0' && flag[i] == flag[0]) {
		repeated_short_arguments = true;
		temp_repetition.push_back(flag[i]);
		i++;
	}

	while (flag[i] != '\0' && flag[i] == flag[0]) {
		i++;
	}

	if (repeated_short_arguments) {
		temp_alias[0] = temp_repetition[0];
		command_line_settings_map[temp_alias]->set_base_variable(temp_repetition.c_str());
	}
		
	// case -abc
	while (flag[i] != '\0') {
		temp_alias[0] = flag[i];

		check_if_option_exists("Unrecognized Option: -", temp_alias, command_line_settings_map.count(temp_alias), true);

		if (command_line_settings_map[temp_alias]->ignored()) {
			std::string error_message;
			error_message.reserve(128);
			error_message += "Order of -";
			error_message += flag[i];
			error_message += " matters, so it cannoth be part of multiple short arguments.";
			throw std::invalid_argument(error_message);
		}
		if (command_line_settings_map[temp_alias]->takes_args()) {
			if (flag[i + 1] != '\0' || cur_argument + 1 >= argc) {
				std::string error_message;
				error_message.reserve(64);
				error_message += "Option requires arguments: ";
				error_message += flag[i];
				throw std::invalid_argument(error_message);
			} else {
				cur_argument++;
				command_line_settings_map[temp_alias]->set_base_variable(argv[cur_argument]);
				break;
			}
		} else {
			command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
		}
		i++;
	}
}


/////////////////////CLI_Interface Definitions////////////////////

inline CLI_Interface::CLI_Interface(void * b_v, std::vector<const char *> a, bool ta, const char * hm) : takes_args_var(ta), base_variable(b_v), aliases(a), help_message(hm) {
	Parser::num_unique_flags += a.size();
	Parser::list_of_cmd_var.push_back(this);
}

inline const std::vector<const char *>& CLI_Interface::get_aliases() const {
	return aliases;
}

inline bool CLI_Interface::takes_args() const {
	return takes_args_var;
}

inline bool CLI_Interface::ignored() const {
	return base_variable == nullptr;
}

inline const char * CLI_Interface::get_help_message() const {
	return help_message;
}

//////////////////////////Var Definitions/////////////////////////

template<typename T>
inline Var<T>::Var(T & b_v, std::vector<const char *> a, bool ta, const char * hm) : CLI_Interface(&b_v, a, ta, hm) {}

template<typename T>
inline Var<T>::Var(T * b_v, std::vector<const char *> a, bool ta, const char * hm) : CLI_Interface(b_v, a, ta, hm) {}

template<typename T>
inline void Var<T>::set_base_variable(const char * b_v) {
	*(T *)base_variable = b_v;
}

///////////////////////Value Definitions//////////////////////

template<typename T>
inline Value<T>::Value(T & b_v, std::vector<const char *>a, T v, const char * hm) : CLI_Interface(&b_v, a, false, hm), value(v) {}

template<typename T>
inline Value<T>::Value(T * b_v, std::vector<const char *>a, T v, const char * hm) : CLI_Interface(b_v, a, false, hm), value(v) {}

template<typename T>
inline void Value<T>::set_base_variable(const char * b_v) {
	(void)b_v;
	*(T*)base_variable = value;
}


//////////////////////Vector Definitions//////////////////////

template<typename T>
inline Vector<T>::Vector(std::vector<T> & b_v, std::vector<const char *>a, const char * hm) : CLI_Interface(&b_v, a, true, hm) {}

template<typename T>
inline Vector<T>::Vector(std::vector<T> * b_v, std::vector<const char *>a, const char * hm) : CLI_Interface(b_v, a, true, hm) {}
	
template<typename T>
inline void Vector<T>::set_base_variable(const char * b_v) {
	std::vector<T>& base_variable_vector = *(std::vector<T> *)base_variable;
	T temp;
	Var<T> temp_var(temp, {}, true);
	temp_var.set_base_variable(b_v);
	base_variable_vector.push_back(temp);
}

/////////////////////////Template Specializations//////////////////////////

inline Var<char>::Var(char * b_v, std::vector<const char *> a, bool ta, int b_s, const char * hm) : CLI_Interface(b_v, a, ta, hm), buffer_size(b_s) {}

inline Var<char>::Var(char & b_v, std::vector<const char *> a, bool ta, const char * hm) : CLI_Interface(&b_v, a, ta, hm), buffer_size(1) {}

inline void Var<char>::set_base_variable(const char * b_v) {
	char * base_variable_string = (char *)base_variable;
	if (buffer_size == 1) {
		*base_variable_string = b_v[0];
		return;
	}
	int i = 0;
	const int adj_buffer_size = buffer_size - 1;
	while (b_v[i] != '\0' && i < adj_buffer_size) {
		base_variable_string[i] = b_v[i];
		i++;
	}
	base_variable_string[i] = '\0';
}


template<>
inline void Var<int>::set_base_variable(const char * b_v) {
	*(int *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Var<unsigned int>::set_base_variable(const char * b_v) {
	*(unsigned int *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Var<long>::set_base_variable(const char * b_v) {
	*(long *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Var<unsigned long>::set_base_variable(const char * b_v) {
	*(unsigned long *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Var<long long>::set_base_variable(const char * b_v) {
	*(long long *)base_variable = strtoll(b_v, nullptr, 10);
}

template<>
inline void Var<unsigned long long>::set_base_variable(const char * b_v) {
	*(unsigned long long *)base_variable = strtoull(b_v, nullptr, 10);
}

template<>
inline void Var<float>::set_base_variable(const char * b_v) {
	*(float *)base_variable = strtof(b_v, nullptr);
}

template<>
inline void Var<double>::set_base_variable(const char * b_v) {
	*(double *)base_variable = strtod(b_v, nullptr);
}

template<>
inline void Var<long double>::set_base_variable(const char * b_v) {
	*(long double *)base_variable = strtold(b_v, nullptr);
}

template<>
inline void Vector<const char *>::set_base_variable(const char * b_v) {
	std::vector<const char *>& base_variable_vector = *(std::vector<const char *>*)base_variable;
	base_variable_vector.push_back(b_v);
}

template<>
inline void Vector<char *>::set_base_variable(const char * b_v) {
	(void)b_v;
	std::string error_message = "Because the length of the char buffers in the vector cannot"
	"be specified and you cannot set a char * to a const char *, you cannot use char"
	"as an acceptable type for a Vector. Use const char *, std::string, or"
	"another template overload.";
	throw std::invalid_argument(Parser::print_within_length_str(error_message));
}

template<>
inline void Vector<char>::set_base_variable(const char * b_v) {
	std::vector<char>& base_variable_vector = *(std::vector<char>*)base_variable;
	while (*b_v != '\0') {
		base_variable_vector.push_back(*b_v);
		b_v++;
	}
}

// Static Declarations

std::vector<CLI_Interface *> Parser::list_of_cmd_var = std::vector<CLI_Interface *>();
std::unordered_map<std::string, CLI_Interface *> Parser::command_line_settings_map = std::unordered_map<std::string, CLI_Interface *>(100);
std::vector<const char *> Parser::non_options = std::vector<const char *>();

std::unordered_map<std::string, Parser::subcommand_func> Parser::subcommand_map = std::unordered_map<std::string, subcommand_func>(10);
std::vector<Parser::subcommand_func> Parser::subcommand_list = std::vector<subcommand_func>();
std::vector<const char *> Parser::subcommand_aliases = std::vector<const char *>();
std::vector<const char *> Parser::subcommand_descriptions = std::vector<const char *>();

size_t Parser::num_unique_flags = 0;

std::string Parser::header = "";
std::string Parser::usage = "";
std::string Parser::footer = "";
std::string Parser::help_file_name = ".main_help_file";
std::string Parser::help_file_path = "`";
size_t Parser::help_width = 80;
std::vector<const char *> Parser::current_command_list = std::vector<const char *>();

}
#endif
