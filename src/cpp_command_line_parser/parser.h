#ifndef CPP_CMD_LINE_PARSER_H
#define CPP_CMD_LINE_PARSER_H
#include "args_parser_templates.h"
#include "hash_table.h"
#include <cstdlib>
#include <stdexcept>
#include <vector>

class ARGS_PARSER {
friend class Command_Line_Var_Interface;
private:
	static Hash_Table<Command_Line_Var_Interface> command_line_settings_map;
	static std::vector<Command_Line_Var_Interface *> list_of_cmd_var;
	static std::vector<const char *> non_options;
private:
	static void fill_hash_table(size_t num_unique_flags);
	static void long_option_handling(char ** argv, int& i);
	static int find_and_mark_split_location(char * flag);
	static void invalid_use_exception_throwing(char * error_message_buffer, const int buffer_size, const int start_of_available_section, const char * offending_flag);

	static void short_option_handling(int argc, char ** argv, int& i);
	static void multiple_short_options_handling(int argc, char ** argv, int& cur_argument);
public:
	static std::vector<const char *> parse(int argc, char ** argv, size_t num_unique_flags = 1000);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////INLINE DECLARATIONS//////////////////////////////
///////////////////////////////////////////////////////////////////////////////

inline std::vector<const char *> ARGS_PARSER::parse(int argc, char ** argv, size_t num_unique_flags) {
	fill_hash_table(num_unique_flags);
	non_options.reserve(100);
	int i = 1;
	for (; i < argc; i++) {
		// cases: --long-option
		if (argv[i][2] != '\0' && argv[i][1] == '-' && argv[i][0] == '-') {
			long_option_handling(argv, i);
		// case: -- and all arguments are options
		} else if (argv[i][0] == '-' && argv[i][1] == '-' && argv[i][2] == '\0') {
			i++;
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
	for (; i < argc; i++) {
		non_options.push_back(argv[i]);
	}
	ARGS_PARSER::list_of_cmd_var.clear();
	return non_options;
}

inline void ARGS_PARSER::fill_hash_table(size_t num_unique_flags) {
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

inline Command_Line_Var_Interface::Command_Line_Var_Interface(void * b_v, std::vector<const char *> a, bool ta) : takes_args_var(ta), base_variable(b_v), aliases(a) {
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

//////////////////////////Command_Line_Var Definitions/////////////////////////

template<typename T>
inline Command_Line_Var<T>::Command_Line_Var(T & b_v, std::vector<const char *> a, bool ta) : Command_Line_Var_Interface(&b_v, a, ta) {}

template<typename T>
inline Command_Line_Var<T>::Command_Line_Var(T * b_v, std::vector<const char *> a, bool ta) : Command_Line_Var_Interface(b_v, a, ta) {}

template<typename T>
inline void Command_Line_Var<T>::set_base_variable(const char * b_v) {
	*(T *)base_variable = b_v;
}

///////////////////////////Template Specializations////////////////////////////

inline Command_Line_Var<char>::Command_Line_Var(char * b_v, std::vector<const char *> a, bool ta, int b_s) : Command_Line_Var_Interface(b_v, a, ta), buffer_size(b_s) {}

inline void Command_Line_Var<char>::set_base_variable(const char * b_v) {
	char * base_variable_string = (char *)base_variable;
	int i = 0;
	while (b_v[i] != '\0' && i < buffer_size) {
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
Hash_Table<Command_Line_Var_Interface> ARGS_PARSER::command_line_settings_map = Hash_Table<Command_Line_Var_Interface>(1000);
std::vector<const char *> ARGS_PARSER::non_options = std::vector<const char *>();

#endif
