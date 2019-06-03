#ifndef CPP_CMD_LINE_PARSER_H
#define CPP_CMD_LINE_PARSER_H
#include "args_parser_templates.h"
#include "template_definitions.h"
#include "cli_help.h"
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <vector>

namespace cli{
	
class Parser {
friend class CLI_Interface;
public:
	typedef void (*subcommand_func)(int, char **, void *);
private:
	std::unordered_map<std::string, CLI_Interface *> command_line_settings_map;
	std::vector<CLI_Interface *> list_of_cmd_var;
	std::vector<const char *> non_options;

	std::unordered_map<std::string, subcommand_func> subcommand_map;
	std::vector<subcommand_func> subcommand_list;
	std::vector<const char *> subcommand_aliases;

	size_t num_unique_flags = 0;

	static CLI_Help help_manager;
private:
	void fill_hash_table();
	void fill_subcommand_hash_table();

	void subcommand_handling(int argc, char ** argv, void * data);

	void long_option_handling(char ** argv, int& i);
	int find_and_mark_split_location(char * flag);
	void check_if_option_exists(const char * error_message, const char * flag, const bool exists, const bool should_exist);

	void short_option_handling(int argc, char ** argv, int& i);
	void multiple_short_options_handling(int argc, char ** argv, int& cur_argument);

	void clear_everything();
	void clear_managed_vars();

	void print_flags();
public:
	~Parser();
	void set_usage(const std::string& u);
	void set_header(const std::string& h);
	void set_footer(const std::string& f);
	void set_help_width(size_t hw);
	void set_help_file_path(const std::string& hfp);
	void generate_help(const char * subcommand_name);
	void print_help();

	void add_subcommand(const char * subcommand, subcommand_func sub_func, const char * description = "");
	void reserve_space_for_subcommand(size_t number_of_subcommand);

	std::vector<const char *> parse(int argc, char ** argv, void * data = nullptr);

	template <typename T>
	CLI_Interface * arg(T& var, std::vector<const char *> flags, const char * help_message = "");

	template <typename T>
	CLI_Interface * value(T& var, std::vector<const char *> flags, T to_set, const char * help_message = "");

	template <typename T>
	CLI_Interface * vector(std::vector<T>& var, std::vector<const char *> flags, const char * help_message = "");

	CLI_Interface * ignored(std::vector<const char *> flags, const char * help_message = "");
	CLI_Interface * repeated(size_t& var, std::vector<const char *> flags, const char * help_message = "");
};

Parser::~Parser() {
	clear_managed_vars();
}


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
	return non_options;
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
	non_options.push_back(nullptr);
	non_options.push_back(argv[0]);
	help_manager.subcommand_descriptions.clear();
	sub_com(argc, argv, data);
}

inline void Parser::reserve_space_for_subcommand(size_t number_of_subcommand) {
	subcommand_list.reserve(number_of_subcommand);
	subcommand_aliases.reserve(number_of_subcommand);
}

inline void Parser::add_subcommand(const char * subcommand, Parser::subcommand_func sub_func, const char * description) {
	subcommand_list.push_back(sub_func);
	subcommand_aliases.push_back(subcommand);
	help_manager.subcommand_descriptions.push_back(description);
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

void Parser::clear_managed_vars() {
	for (size_t i = 0; i < list_of_cmd_var.size(); i++) {
		delete list_of_cmd_var[i];
	}
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

	if (repeated_short_arguments) {
		temp_alias[0] = temp_repetition[0];
		command_line_settings_map[temp_alias]->set_base_variable(std::to_string(temp_repetition.length()).c_str());
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

void Parser::set_usage(const std::string& u) {
	help_manager.set_usage(u);
}
void Parser::set_header(const std::string& h) {
	help_manager.set_header(h);
}
void Parser::set_footer(const std::string& f) {
	help_manager.set_footer(f);
}
void Parser::set_help_width(size_t hw) {
	help_manager.set_help_width(hw);
}
void Parser::set_help_file_path(const std::string& hfp) {
	help_manager.set_help_file_path(hfp);
}
void Parser::generate_help(const char * subcommand_name) {
	help_manager.generate_help(subcommand_name, subcommand_aliases, list_of_cmd_var);
}
void Parser::print_help() {
	help_manager.print_help();
}

template <typename T>
CLI_Interface * Parser::arg(T& var, std::vector<const char *> flags, const char * help_message) {
	list_of_cmd_var.push_back(new Var<T>(var, flags, true, help_message));
	return list_of_cmd_var.back();
}

template <typename T>
CLI_Interface * Parser::value(T& var, std::vector<const char *> flags, T to_set, const char * help_message) {
	list_of_cmd_var.push_back(new Value<T>(var, flags, to_set, help_message));
	return list_of_cmd_var.back();
}

template <typename T>
CLI_Interface * Parser::vector(std::vector<T>& var, std::vector<const char *> flags, const char * help_message) {
	list_of_cmd_var.push_back(new Vector<T>(var, flags, help_message));
	return list_of_cmd_var.back();
}

CLI_Interface * Parser::ignored(std::vector<const char *> flags, const char * help_message) {
	list_of_cmd_var.push_back(new Var<int>(nullptr, flags, false, help_message));
	return list_of_cmd_var.back();
}

CLI_Interface * Parser::repeated(size_t& var, std::vector<const char *> flags, const char * help_message) {
	list_of_cmd_var.push_back(new Var<size_t>(var, flags, false, help_message));
	return list_of_cmd_var.back();
}
CLI_Help Parser::help_manager = CLI_Help();
}
#endif
