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
	static std::vector<Command_Line_Var_Interface *> list_of_cmd_var;
public:
	static std::vector<const char *> parse(int argc, char ** argv, size_t num_unique_flags = 1000);
};

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////INLINE DECLARATIONS//////////////////////////////
///////////////////////////////////////////////////////////////////////////////

inline std::vector<const char *> ARGS_PARSER::parse(int argc, char ** argv, size_t num_unique_flags) {
	Hash_Table<Command_Line_Var_Interface> command_line_settings_map(num_unique_flags);
	std::vector<const char *> non_options;
	non_options.reserve(100);
	for (size_t i = 0; i < ARGS_PARSER::list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * cur_com_var = ARGS_PARSER::list_of_cmd_var[i];
		const std::vector<const char *> & cur_aliases = cur_com_var->get_aliases();
		for (size_t j = 0; j < cur_aliases.size(); j++) {
			command_line_settings_map.insert(cur_aliases[j], cur_com_var);
		}
	}
	int i = 1;
	for (; i < argc; i++) {
		// cases: --long-option
		if (argv[i][2] != '\0' && argv[i][1] == '-' && argv[i][0] == '-') {
			char * temp_alias = argv[i] + 2;
			int split_location = 0;
			for (; temp_alias[split_location] != '\0'; split_location++) {
				if (temp_alias[split_location] == '=') {
					temp_alias[split_location] = '\0';
					split_location++;
					break;
				}
			}
			if (command_line_settings_map.count(temp_alias) == 0) {
				char error_message[54] = "Unrecognized Option: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
				int error_message_iterator = 21;
				while (error_message_iterator < 53 && temp_alias[error_message_iterator - 21] != '\0') {
					error_message[error_message_iterator] = temp_alias[error_message_iterator - 21];
					error_message_iterator++;
				}
				throw std::invalid_argument(error_message);
			}
			// case: --long-option=value
			if (temp_alias[split_location] != '\0') {
				if (command_line_settings_map[temp_alias]->takes_args()) {
					command_line_settings_map[temp_alias]->set_base_variable(temp_alias + split_location);
				} else {
					char error_message[] = "Option does not take arguments: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
					int error_message_iterator = 32;
					while (error_message_iterator < 64 && temp_alias[error_message_iterator - 32] != '\0') {
						error_message[error_message_iterator] = temp_alias[error_message_iterator - 32];
						error_message_iterator++;
					}
					throw std::invalid_argument(error_message);
				}
			// case: --long-option
			} else {
				command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
			}
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
			int j = 1;
			char temp_alias[2] = "\0";
			temp_alias[0] = argv[i][1];
			if (command_line_settings_map.count(temp_alias) == 0) {
				char error_message[21 + 2] = "Unrecognized Option: \0";
				error_message[21] = argv[i][1];
				throw std::invalid_argument(error_message);
			}
			// case: -o value
			if (argv[i][2] == '\0' && i + 1 < argc && command_line_settings_map.count(argv[i] + 1) != 0 && command_line_settings_map[argv[i] + 1]->takes_args()) {
				command_line_settings_map[argv[i] + 1]->set_base_variable(argv[i + 1]);
				i++;
				continue;
			}
			// case: -oValue
			bool multiple_short_arguments = true;
			for (int k = j; argv[i][k] != '\0'; k++) {
				char temp_string[2] = "\0";
				temp_string[0] = argv[i][k];
				if (command_line_settings_map.count(temp_string) == 0) {
					temp_alias[0] = argv[i][j];
					if (command_line_settings_map[temp_alias]->takes_args()) {
						command_line_settings_map[temp_alias]->set_base_variable(argv[i] + 2);
					} else {
						char error_message[] = "Option does not take arguments: \0";
						error_message[32] = argv[i][1];
						throw std::invalid_argument(error_message);
					}
					multiple_short_arguments = false;
					break;
				}
			}

			// case -vvv
			char temp_repetition[12] = "\0\0\0\0\0\0\0\0\0\0\0";
			bool repeated_short_arguments = false;
			while (multiple_short_arguments && argv[i][j] != '\0' && argv[i][j] == argv[i][1] && j < 12) {
				repeated_short_arguments = true;
				temp_repetition[j - 1] = argv[i][j];
				j++;
			}
			if (repeated_short_arguments) {
				temp_alias[0] = temp_repetition[0];
				command_line_settings_map[temp_alias]->set_base_variable(temp_repetition);
			}
				
			// case -abc
			while (multiple_short_arguments && argv[i][j] != '\0') {
				temp_alias[0] = argv[i][j];
				command_line_settings_map[temp_alias]->set_base_variable(temp_alias);
				j++;
			}
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

//////////////////////////Command_Line_Var Definitions/////////////////////////

template<typename T>
inline Command_Line_Var<T>::Command_Line_Var(T * b_v, std::vector<const char *> a, bool ta) : Command_Line_Var_Interface(b_v, a, ta) {}

template<typename T>
inline void Command_Line_Var<T>::set_base_variable(const char * b_v) {
	*(T *)base_variable = b_v;
}

///////////////////////////Template Specializations////////////////////////////

inline Command_Line_Var<char>::Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s) : Command_Line_Var_Interface(b_v, a, ta), buffer_size(b_s) {}

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
void Command_Line_Var<long>::set_base_variable(const char * b_v) {
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

#endif
