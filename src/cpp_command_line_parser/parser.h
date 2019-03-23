#ifndef CPP_CMD_LINE_PARSER_H
#define CPP_CMD_LINE_PARSER_H
#include "args_parser_templates.h"
#include "hash_table.h"
#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <cstdio>

class ARGS_PARSER {
friend class Command_Line_Var_Interface;
public:
	typedef void (*subcommand_func)(int, char **, void *);
private:
	static Hash_Table<Command_Line_Var_Interface> command_line_settings_map;
	static std::vector<Command_Line_Var_Interface *> list_of_cmd_var;
	static std::vector<const char *> non_options;

	static Hash_Table<subcommand_func> subcommand_map;
	static std::vector<subcommand_func> subcommand_list;
	static std::vector<const char *> subcommand_aliases;
	static std::vector<const char *> subcommand_descriptions;

	static size_t num_unique_flags;

	static const char * header;
	static const char * usage;
	static const char * footer;
	static const char * help_file_name;
	static int help_width;
private:
	static void fill_hash_table();
	static void fill_subcommand_hash_table();

	static void subcommand_handling(int argc, char ** argv, void * data);

	static void long_option_handling(char ** argv, int& i);
	static int find_and_mark_split_location(char * flag);
	static void invalid_use_exception_throwing(char * error_message_buffer, const int buffer_size, const int start_of_edit_section, const char * offending_flag);

	static void short_option_handling(int argc, char ** argv, int& i);
	static void multiple_short_options_handling(int argc, char ** argv, int& cur_argument);

	static void clear_everything();
	static void clear_memory();

	static void print_within_length(const char * str, int indent = 0, FILE * file_writer = stdout);
	static void print_flags();
public:
	static void add_subcommand(const char * subcommand, subcommand_func sub_func, const char * description = "");
	static void reserve_space_for_subcommand(size_t number_of_subcommand);

	static void set_header(const char * h);
	static void set_usage(const char * u);
	static void set_footer(const char * f);
	static void set_help_width(int hw);
	static void set_help_file_name(const char * hfn);

	static void generate_help();
	static void print_help();
	
	static std::vector<const char *> parse(int argc, char ** argv, void * data = nullptr);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////INLINE DECLARATIONS//////////////////////////////
///////////////////////////////////////////////////////////////////////////////

inline std::vector<const char *> ARGS_PARSER::parse(int argc, char ** argv, void * data) {
	fill_hash_table();
	fill_subcommand_hash_table();
	non_options.reserve(2 * argc);
	int i = 1;
	for (; i < argc; i++) {
		// case: subcommand, which is recursive
		if (subcommand_map.count(argv[i]) != 0) {
			char buffer[1024];
			sprintf(buffer, ".%s_help_file", argv[i]);
			set_help_file_name(buffer);
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
		} else if (argv[i][0] == '-' && argv[i][1] == '\0') {
			if (command_line_settings_map.count("-") != 0) {
				command_line_settings_map["-"]->set_base_variable("-");
			}
		} else if (argv[i][0] == '-') {
			short_option_handling(argc, argv, i);
		} else {
			non_options.push_back(argv[i]);
		}
	}
	clear_memory();
	return non_options;
}

inline void ARGS_PARSER::set_header(const char * h) {
	header = h;
}

inline void ARGS_PARSER::set_usage(const char * u) {
	usage = u;
}

inline void ARGS_PARSER::set_footer(const char * f) {
	footer = f;
}

inline void ARGS_PARSER::set_help_width(int hw) {
	help_width = hw;
}

inline void ARGS_PARSER::set_help_file_name(const char * hfn) {
	help_file_name = hfn;
}

inline void ARGS_PARSER::generate_help() {
	FILE * file_exists = fopen(help_file_name, "r");
	if (file_exists != nullptr) {
		fclose(file_exists);
		return;
	}

	FILE * file_writer = fopen(help_file_name, "w");
	if (usage[0] != '\0') {
		print_within_length(usage, 0, file_writer);
		fprintf(file_writer, "\n");
	}
	if (header[0] != '\0') {
		print_within_length(header, 0, file_writer);
		fprintf(file_writer, "\n");
	}

	bool any_descriptive_subcommands = false;

	for (size_t i = 0; !any_descriptive_subcommands && i < subcommand_descriptions.size(); i++) {
		any_descriptive_subcommands = subcommand_descriptions[i][0] != '`';
	}

	if (any_descriptive_subcommands) {
		fprintf(file_writer, "SUBCOMMANDS:\n");
		for (size_t i = 0; i < subcommand_descriptions.size(); i++) {
			if (subcommand_descriptions[i][0] != '`') {
				fprintf(file_writer, "%s\n", subcommand_aliases[i]);
				print_within_length(subcommand_descriptions[i], 8, file_writer);
			}
		}
		fprintf(file_writer, "\n");
	}


	fprintf(file_writer, "OPTIONS:\n");
	for (size_t i = 0; i < list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * clv = list_of_cmd_var[i];
		const std::vector<const char *>& aliases = clv->get_aliases();
		if (clv->get_help_message()[0] != '`') {
			for (size_t j = 0; j < aliases.size(); j++) {
				if (aliases[j][0] == '-' && aliases[j][1] == '\0') {
					fprintf(file_writer, "-, ");
				} else if (aliases[j][1] == '\0') {
					fprintf(file_writer, "-%s, ", aliases[j]);
				} else {
					fprintf(file_writer, "--%s, ", aliases[j]);
				}
			}
			fprintf(file_writer, "\b\b  \b\b\n");
			print_within_length(clv->get_help_message(), 8, file_writer);
		}
	}	
	fprintf(file_writer, "\n");
	if (footer[0] != '\0') {
		print_within_length(footer, 0, file_writer);
	}
	fclose(file_writer);
}

inline void ARGS_PARSER::print_help() {
	FILE * file_reader = fopen(help_file_name, "r");
	if (file_reader == nullptr) {
		char error_buffer[1024];
		sprintf(error_buffer, "%s has not been generated. "
		"Please put ARGS_PARSER::generate_help() right before calling ARGS_PARSER::parse in the subcommand: ", help_file_name);
		int i = 1;
		int offset = strlen(error_buffer);
		int max_length_of_name = strlen(help_file_name) - strlen("_help_file");
		while (i < max_length_of_name && help_file_name[i] != '\0') {
			error_buffer[i - 1 + offset] = help_file_name[i];
			i++;
		}
		error_buffer[i - 1 + offset] = '\0';
		print_within_length(error_buffer, 0, stderr);
		return;
	}
	char buffer[2048];
	size_t nread;
	if (file_reader) {
		while ((nread = fread(buffer, 1, sizeof buffer, file_reader)) > 0) {
			fwrite(buffer, 1, nread, stdout);
		}
	}
	fclose(file_reader);
}
	

inline void ARGS_PARSER::print_within_length(const char * str, int indent, FILE * file_writer) {
	int i = 0;
	int last_position = -1;
	while (str[i] != '\0') {
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
			fprintf(file_writer, "%*s\n", indent + i, str);
		} else {
			fprintf(file_writer, "%*.*s\n", last_position + indent, last_position, str);
			str += last_position + 1;
			i = 0;
		}
	}
}

inline void ARGS_PARSER::fill_hash_table() {
	command_line_settings_map = Hash_Table<Command_Line_Var_Interface>(num_unique_flags);
	Hash_Table<int> flag_already_used(num_unique_flags);
	for (size_t i = 0; i < ARGS_PARSER::list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * cur_com_var = list_of_cmd_var[i];
		const std::vector<const char *> & cur_aliases = cur_com_var->get_aliases();
		for (size_t j = 0; j < cur_aliases.size(); j++) {
			if (flag_already_used.count(cur_aliases[j])) {
				const int buffer_size = 52;
				const int start_of_available_section = 19;
				char error_message[buffer_size] = "Flag already used: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
				invalid_use_exception_throwing(error_message, buffer_size, start_of_available_section, cur_aliases[j]);
			}
			flag_already_used.insert(cur_aliases[j], nullptr);
			command_line_settings_map.insert(cur_aliases[j], cur_com_var);
		}
	}
}

inline void ARGS_PARSER::subcommand_handling(int argc, char ** argv, void * data) {
	subcommand_func sub_com = *(subcommand_map[argv[0]]);
	clear_everything();
	non_options.push_back(nullptr);
	non_options.push_back(argv[0]);
	sub_com(argc, argv, data);
}

inline void ARGS_PARSER::reserve_space_for_subcommand(size_t number_of_subcommand) {
	subcommand_list.reserve(number_of_subcommand);
	subcommand_aliases.reserve(number_of_subcommand);
}

inline void ARGS_PARSER::add_subcommand(const char * subcommand, ARGS_PARSER::subcommand_func sub_func, const char * description) {
	subcommand_list.push_back(sub_func);
	subcommand_aliases.push_back(subcommand);
	subcommand_descriptions.push_back(description);
}

inline void ARGS_PARSER::fill_subcommand_hash_table() {
	size_t n_sub = subcommand_aliases.size();
	subcommand_map.reserve(2 * n_sub);
	Hash_Table<int> flag_already_used(2 * n_sub);
	for (size_t i = 0; i < n_sub; i++) {
		if (flag_already_used.count(subcommand_aliases[i])) {
			const int buffer_size = 58;
			const int start_of_available_section = 25;
			char error_message[buffer_size] = "Subcommand already used: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
			invalid_use_exception_throwing(error_message, buffer_size, start_of_available_section, subcommand_aliases[i]);
		}
		flag_already_used.insert(subcommand_aliases[i], nullptr);
		subcommand_map.insert(subcommand_aliases[i], &(subcommand_list[i]));
	}
}

inline void ARGS_PARSER::clear_everything() {
	subcommand_map.clear();
	subcommand_list.clear();
	subcommand_aliases.clear();
	subcommand_descriptions.clear();
	list_of_cmd_var.clear();
	command_line_settings_map.clear();
	num_unique_flags = 0;
	// non_options is never deleted.
}

inline void ARGS_PARSER::clear_memory() {
	subcommand_map.clear_memory();
	command_line_settings_map.clear_memory();

	std::vector<subcommand_func>().swap(subcommand_list);
	std::vector<const char *>().swap(subcommand_aliases);
	std::vector<Command_Line_Var_Interface *>().swap(list_of_cmd_var);
	std::vector<const char *>().swap(subcommand_descriptions);
	num_unique_flags = 0;
}

inline int ARGS_PARSER::find_and_mark_split_location(char * flag) {
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

inline void ARGS_PARSER::invalid_use_exception_throwing(char * error_message_buffer, const int buffer_size, const int start_of_available_section, const char * offending_flag) {
	int error_message_iterator = start_of_available_section;
	while (error_message_iterator < buffer_size - 1 && offending_flag[error_message_iterator - start_of_available_section] != '\0') {
		error_message_buffer[error_message_iterator] = offending_flag[error_message_iterator - start_of_available_section];
		error_message_iterator++;
	}
	throw std::invalid_argument(error_message_buffer);
}

inline void ARGS_PARSER::long_option_handling(char ** argv, int& i) {
	char * temp_alias = argv[i] + 2;
	int split_location = find_and_mark_split_location(temp_alias);

	if (command_line_settings_map.count(temp_alias) == 0) {
		const int buffer_size = 54;
		const int start_of_available_section = 21;
		char error_message[buffer_size] = "Unrecognized Option: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		invalid_use_exception_throwing(error_message, buffer_size, start_of_available_section, temp_alias);
	}

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
			const int buffer_size = 65;
			const int start_of_available_section = 32;
			char error_message[buffer_size] = "Option does not take arguments: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
			invalid_use_exception_throwing(error_message, buffer_size, start_of_available_section, temp_alias);
		}
	// case: --long-option
	} else if (command_line_settings_map[temp_alias]->takes_args()) {
		const int buffer_size = 60;
		const int start_of_available_section = 27;
		char error_message[buffer_size] = "Option requires arguments: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		invalid_use_exception_throwing(error_message, buffer_size, start_of_available_section, temp_alias);
	} else {
		command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
	}
}

inline void ARGS_PARSER::short_option_handling(int argc, char ** argv, int& i) {
	char temp_alias[2] = "\0";
	temp_alias[0] = argv[i][1];
	if (command_line_settings_map.count(temp_alias) == 0) {
		char error_message[] = "Unrecognized Option: \0";
		error_message[21] = argv[i][1];
		throw std::invalid_argument(error_message);
	}

	if (command_line_settings_map[temp_alias]->ignored()) {
		non_options.push_back(argv[i]);
		return;
	}
	// case: -o value
	if (argv[i][2] == '\0' && i + 1 < argc && command_line_settings_map.count(argv[i] + 1) != 0 && command_line_settings_map[argv[i] + 1]->takes_args()) {
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

inline void ARGS_PARSER::multiple_short_options_handling(int argc, char ** argv, int& cur_argument) {
	int i = 0;
	char * flag = argv[cur_argument] + 1;
	char temp_alias[2] = "\0";

	// case -vvv
	const int repetition_buffer_size = 32;
	char temp_repetition[repetition_buffer_size] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	bool repeated_short_arguments = false;
	while (flag[i] != '\0' && flag[i] == flag[0] && i < repetition_buffer_size - 1) {
		repeated_short_arguments = true;
		temp_repetition[i] = flag[i];
		i++;
	}

	while (flag[i] != '\0' && flag[i] == flag[0]) {
		i++;
	}

	if (repeated_short_arguments) {
		temp_alias[0] = temp_repetition[0];
		command_line_settings_map[temp_alias]->set_base_variable(temp_repetition);
	}
		
	// case -abc
	while (flag[i] != '\0') {
		temp_alias[0] = flag[i];
		if (command_line_settings_map.count(temp_alias) == 0) {
			char error_message[] = "Unrecognized Option: \0";
			error_message[21] = argv[i][1];
			throw std::invalid_argument(error_message);
		}
		if (command_line_settings_map[temp_alias]->ignored()) {
			char error_message[] = "Order of \"-\0\" matters, so it cannot be part of multiple short arguments.";
			error_message[11] = flag[i];
			throw std::invalid_argument(error_message);
		}
		if (command_line_settings_map[temp_alias]->takes_args()) {
			if (flag[i + 1] != '\0' || cur_argument + 1 >= argc) {
				char error_message[] = "Option requires arguments: \0";
				error_message[27] = flag[i];
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


/////////////////////Command_Line_Var_Interface Definitions////////////////////

inline Command_Line_Var_Interface::Command_Line_Var_Interface(void * b_v, std::vector<const char *> a, bool ta, const char * hm) : takes_args_var(ta), base_variable(b_v), aliases(a), help_message(hm) {
	ARGS_PARSER::num_unique_flags += a.size();
	ARGS_PARSER::list_of_cmd_var.push_back(this);
}

inline const std::vector<const char *>& Command_Line_Var_Interface::get_aliases() const {
	return aliases;
}

inline bool Command_Line_Var_Interface::takes_args() const {
	return takes_args_var;
}

inline bool Command_Line_Var_Interface::ignored() const {
	return base_variable == nullptr;
}

inline const char * Command_Line_Var_Interface::get_help_message() const {
	return help_message;
}

//////////////////////////Command_Line_Var Definitions/////////////////////////

template<typename T>
inline Command_Line_Var<T>::Command_Line_Var(T & b_v, std::vector<const char *> a, bool ta, const char * hm) : Command_Line_Var_Interface(&b_v, a, ta, hm) {}

template<typename T>
inline Command_Line_Var<T>::Command_Line_Var(T * b_v, std::vector<const char *> a, bool ta, const char * hm) : Command_Line_Var_Interface(b_v, a, ta, hm) {}

template<typename T>
inline void Command_Line_Var<T>::set_base_variable(const char * b_v) {
	*(T *)base_variable = b_v;
}

///////////////////////Command_Line_Value Definitions//////////////////////

template<typename T>
inline Command_Line_Value<T>::Command_Line_Value(T & b_v, std::vector<const char *>a, T v, const char * hm) : Command_Line_Var_Interface(&b_v, a, false, hm), value(v) {}

template<typename T>
inline Command_Line_Value<T>::Command_Line_Value(T * b_v, std::vector<const char *>a, T v, const char * hm) : Command_Line_Var_Interface(b_v, a, false, hm), value(v) {}

template<typename T>
inline void Command_Line_Value<T>::set_base_variable(const char * b_v) {
	(void)b_v;
	*(T*)base_variable = value;
}

///////////////////////////Template Specializations////////////////////////////

inline Command_Line_Var<char>::Command_Line_Var(char * b_v, std::vector<const char *> a, bool ta, int b_s, const char * hm) : Command_Line_Var_Interface(b_v, a, ta, hm), buffer_size(b_s) {}

inline Command_Line_Var<char>::Command_Line_Var(char & b_v, std::vector<const char *> a, bool ta, const char * hm) : Command_Line_Var_Interface(&b_v, a, ta, hm), buffer_size(1) {}

inline void Command_Line_Var<char>::set_base_variable(const char * b_v) {
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
inline void Command_Line_Var<int>::set_base_variable(const char * b_v) {
	*(int *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<unsigned int>::set_base_variable(const char * b_v) {
	*(unsigned int *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<long>::set_base_variable(const char * b_v) {
	*(long *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<unsigned long>::set_base_variable(const char * b_v) {
	*(unsigned long *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<long long>::set_base_variable(const char * b_v) {
	*(long long *)base_variable = strtoll(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<unsigned long long>::set_base_variable(const char * b_v) {
	*(unsigned long long *)base_variable = strtoull(b_v, nullptr, 10);
}

template<>
inline void Command_Line_Var<float>::set_base_variable(const char * b_v) {
	*(float *)base_variable = strtof(b_v, nullptr);
}

template<>
inline void Command_Line_Var<double>::set_base_variable(const char * b_v) {
	*(double *)base_variable = strtod(b_v, nullptr);
}

template<>
inline void Command_Line_Var<long double>::set_base_variable(const char * b_v) {
	*(long double *)base_variable = strtold(b_v, nullptr);
}

std::vector<Command_Line_Var_Interface *> ARGS_PARSER::list_of_cmd_var = std::vector<Command_Line_Var_Interface *>();
Hash_Table<Command_Line_Var_Interface> ARGS_PARSER::command_line_settings_map = Hash_Table<Command_Line_Var_Interface>(100);
std::vector<const char *> ARGS_PARSER::non_options = std::vector<const char *>();

Hash_Table<ARGS_PARSER::subcommand_func> ARGS_PARSER::subcommand_map = Hash_Table<subcommand_func>(10);
std::vector<ARGS_PARSER::subcommand_func> ARGS_PARSER::subcommand_list = std::vector<subcommand_func>();
std::vector<const char *> ARGS_PARSER::subcommand_aliases = std::vector<const char *>();
std::vector<const char *> ARGS_PARSER::subcommand_descriptions = std::vector<const char *>();

size_t ARGS_PARSER::num_unique_flags = 0;

const char * ARGS_PARSER::header = "";
const char * ARGS_PARSER::usage = "";
const char * ARGS_PARSER::footer = "";
const char * ARGS_PARSER::help_file_name = ".main_help_file";
int ARGS_PARSER::help_width = 80;

#endif
