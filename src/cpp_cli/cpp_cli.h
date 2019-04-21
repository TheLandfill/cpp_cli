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
	static const char * help_file_path;
	static int help_width;
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

	static void print_within_length(const char * str, int indent = 0, FILE * file_writer = stdout);
	static int print_within_length(const char * str, char * buffer, int bf_size, int indent = 0);
	static void print_flags();
public:
	static void add_subcommand(const char * subcommand, subcommand_func sub_func, const char * description = "");
	static void reserve_space_for_subcommand(size_t number_of_subcommand);

	static void set_usage(const char * u);
	static void set_header(const char * h);
	static void set_footer(const char * f);
	static void set_help_width(int hw);
	static void set_help_file_name(const char * hfn);
	static void set_help_file_path(const char * hfp);

	static void generate_help(const char * command_name);
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

inline void ARGS_PARSER::set_help_file_path(const char * hfp) {
	help_file_path = hfp;
}

inline void ARGS_PARSER::generate_help(const char * subcommand_name) {
	if (subcommand_name[0] == '.' && subcommand_name[1] == '/') {
		subcommand_name += 2;
	}
	current_command_list.push_back(subcommand_name);
	char buffer[2048];
	char * buffer_index = buffer;
	if (help_file_path == nullptr) {
		print_within_length("The help file path has not been set. "
		"Use the command 'ARGS_PARSER::set_help_file_path(const char * hfn)' to set a valid file path before calling generate_help. "
		"The file path should be an absolute path if you want the program to run anywhere. "
		"Only use a relative path if your executable can only be executed from one spot.", buffer, 2048);
		throw std::runtime_error(buffer);
	}
	buffer_index += sprintf(buffer_index, "%s.", help_file_path);
	for (size_t i = 0; i < current_command_list.size(); i++) {
		buffer_index += sprintf(buffer_index, "%s_", current_command_list[i]);
	}
	sprintf(buffer_index, "help_file");
	set_help_file_name(buffer);

	FILE * file_exists = fopen(help_file_name, "r");
	if (file_exists != nullptr) {
		fclose(file_exists);
		return;
	}

	FILE * file_writer = fopen(help_file_name, "w");
	if (file_writer == nullptr) {
		char error_message_buffer[1024];
		sprintf(error_message_buffer, "%s must exist and be accessable by the current user.", help_file_path);
		throw std::runtime_error(error_message_buffer);
	}

	char usage_buffer[2048];
	char * usage_buffer_index = usage_buffer;
	usage_buffer_index += sprintf(usage_buffer_index, "%s", "usage:");
	for (size_t i = 0; i < current_command_list.size(); i++) {
		usage_buffer_index += sprintf(usage_buffer_index, " %s", current_command_list[i]);
	}
	sprintf(usage_buffer_index, " %s", usage);

	print_within_length(usage_buffer, 0, file_writer);
	print_within_length(header, 0, file_writer);

	bool any_descriptions = false;

	for (size_t i = 0; !any_descriptions && i < subcommand_descriptions.size(); i++) {
		any_descriptions = subcommand_descriptions[i][0] != '`';
	}

	if (any_descriptions) {
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
	char help_buffer[2048];
	for (size_t i = 0; i < list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * clv = list_of_cmd_var[i];
		const std::vector<const char *>& a = clv->get_aliases();
		if (clv->get_help_message()[0] != '`') {
			int len_str = 0;
			const char * n_dash = "--";
			for (size_t j = 0; j < a.size(); j++) {
				int added = sprintf(help_buffer + len_str, "%s%s, ", n_dash + (a[j][1] == '\0') * (1 + (a[j][0] == '-')), a[j]);
				len_str += added;
			}
			help_buffer[len_str - 2] = '\0';

			fprintf(file_writer, "%s\n", help_buffer);
			print_within_length(clv->get_help_message(), 8, file_writer);
		}
	}	
	fprintf(file_writer, "\n");
	print_within_length(footer, 0, file_writer);
	fclose(file_writer);
}

inline void ARGS_PARSER::print_help() {
	FILE * file_reader = fopen(help_file_name, "r");
	if (file_reader == nullptr) {
		char error_message[1024];
		sprintf(error_message, "%s has not been generated. "
		"Please put ARGS_PARSER::generate_help() right before calling ARGS_PARSER::parse in the subcommand: ", help_file_name);
		int i = 1;
		int offset = strlen(error_message);
		int max_length_of_name = strlen(help_file_name) - strlen("_help_file");
		while (i < max_length_of_name && help_file_name[i] != '\0') {
			error_message[i - 1 + offset] = help_file_name[i];
			i++;
		}
		error_message[i - 1 + offset] = '\0';
		char error_buffer[2048];
		print_within_length(error_message, error_buffer, 2048);
		throw std::runtime_error(error_buffer);
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

inline int ARGS_PARSER::print_within_length(const char * str, char * buffer, int bf_size, int indent) {
	int i = 0;
	int last_position = -1;
	const char * initial_str = str;
	while (str[i] != '\0' && str - initial_str < bf_size - help_width - 1) {
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
			buffer += sprintf(buffer, "%*s\n", indent + i, str);
			str += i;
		} else {
			buffer += sprintf(buffer, "%*.*s\n", last_position + indent, last_position, str);
			str += last_position + 1;
		}
		i = 0;
	}
	return str - initial_str;
}

inline void ARGS_PARSER::print_within_length(const char * str, int indent, FILE * file_writer) {
	const int bf_size = 2048;
	char buffer[bf_size];
	const char * i_str = str;
	while (str[0] != '\0') {
		str += print_within_length(str, buffer, bf_size, indent);
		fprintf(file_writer, "%s", buffer);
	}
	if (i_str[0] != '\0') {
		fprintf(file_writer, "\n");
	}
}

inline void ARGS_PARSER::fill_hash_table() {
	command_line_settings_map = Hash_Table<Command_Line_Var_Interface>(num_unique_flags);
	Hash_Table<int> flag_already_used(num_unique_flags);
	for (size_t i = 0; i < ARGS_PARSER::list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * cur_com_var = list_of_cmd_var[i];
		const std::vector<const char *> & cur_aliases = cur_com_var->get_aliases();
		for (size_t j = 0; j < cur_aliases.size(); j++) {
			check_if_option_exists("Flag already used: ", cur_aliases[j], flag_already_used.count(cur_aliases[j]), false);
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
		check_if_option_exists("Subcommand already used: ", subcommand_aliases[i], flag_already_used.count(subcommand_aliases[i]), false);
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

inline void ARGS_PARSER::check_if_option_exists(const char * error_message, const char * potential_option, const bool exists, const bool should_exist) {
	char error_message_buffer[1024];
	if (exists != should_exist) {
		sprintf(error_message_buffer, "%s%s", error_message, potential_option);
		throw std::invalid_argument(error_message_buffer);
	}
}

inline void ARGS_PARSER::long_option_handling(char ** argv, int& i) {
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
			char error_message_buffer[1024];
			sprintf(error_message_buffer, "%s%s", "Option does not take arguments: --", temp_alias);
			throw std::invalid_argument(error_message_buffer);
		}
	// case: --long-option
	} else if (command_line_settings_map[temp_alias]->takes_args()) {
		char error_message_buffer[1024];
		sprintf(error_message_buffer, "%s%s", "Option requires arguments: --", temp_alias);
		throw std::invalid_argument(error_message_buffer);
	} else {
		command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
	}
}

inline void ARGS_PARSER::short_option_handling(int argc, char ** argv, int& i) {
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

		check_if_option_exists("Unrecognized Option: -", temp_alias, command_line_settings_map.count(temp_alias), true);

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


///////////////////////Command_Line_Value Definitions//////////////////////

template<typename T>
inline Command_Line_Vector<T>::Command_Line_Vector(std::vector<T> & b_v, std::vector<const char *>a, const char * hm) : Command_Line_Var_Interface(&b_v, a, true, hm) {}

template<typename T>
inline Command_Line_Vector<T>::Command_Line_Vector(std::vector<T> * b_v, std::vector<const char *>a, const char * hm) : Command_Line_Var_Interface(b_v, a, true, hm) {}
	
template<typename T>
inline void Command_Line_Vector<T>::set_base_variable(const char * b_v) {
	std::vector<T>& base_variable_vector = *(std::vector<T> *)base_variable;
	T temp;
	Command_Line_Var<T> temp_var(temp, {}, true);
	temp_var.set_base_variable(b_v);
	base_variable_vector.push_back(temp);
}

/////////////////////////Template Specializations//////////////////////////

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

template<>
inline void Command_Line_Vector<const char *>::set_base_variable(const char * b_v) {
	std::vector<const char *>& base_variable_vector = *(std::vector<const char *>*)base_variable;
	base_variable_vector.push_back(b_v);
}

template<>
inline void Command_Line_Vector<char *>::set_base_variable(const char * b_v) {
	(void)b_v;
	char error_message[] = "Because the length of the char buffers in the vector cannot\n"
	"be specified and you cannot set a char * to a const char *, you cannot use char\n"
	"as an acceptable type for a Command_Line_Vector. Use const char *, std::string, or\n"
	"another template overload.";
	throw std::invalid_argument(error_message);
}

template<>
inline void Command_Line_Vector<char>::set_base_variable(const char * b_v) {
	std::vector<char>& base_variable_vector = *(std::vector<char>*)base_variable;
	base_variable_vector.push_back(b_v[0]);
}

// Static Declarations

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
const char * ARGS_PARSER::help_file_path = nullptr;
int ARGS_PARSER::help_width = 80;
std::vector<const char *> ARGS_PARSER::current_command_list = std::vector<const char *>();

#endif
