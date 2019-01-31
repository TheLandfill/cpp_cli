#include <cstdlib>
#include "../includes/parser.h"
#include "../includes/hash_table.h"

static std::vector<Command_Line_Var_Interface *> list_of_cmd_var;

Command_Line_Var_Interface::Command_Line_Var_Interface(void * b_v, std::vector<const char *> a, bool ta) : takes_args_var(ta), base_variable(b_v), aliases(a) {
	list_of_cmd_var.push_back(this);
}

bool Command_Line_Var_Interface::takes_args() {
	return takes_args_var;
}

std::vector<const char *> hash_cmd_line_into_variables(int argc, char ** argv, size_t num_unique_flags) {
	Hash_Table command_line_settings_map(num_unique_flags);
	std::vector<const char *> non_options;
	non_options.reserve(100);
	for (size_t i = 0; i < list_of_cmd_var.size(); i++) {
		Command_Line_Var_Interface * cur_com_var = list_of_cmd_var[i];
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
			// case: --long-option=value
			if (temp_alias[split_location] != '\0') {
				command_line_settings_map[temp_alias]->set_base_variable(temp_alias + split_location);
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
			// case: -o value
			if (argv[i][2] == '\0' && i + 1 < argc && command_line_settings_map.count(argv[i] + 1) != 0 && command_line_settings_map[argv[i] + 1]->takes_args()) {
				command_line_settings_map[argv[i] + 1]->set_base_variable(argv[i + 1]);
				i++;
				continue;
			}
			// case: -oValue
			char temp_alias[2] = "\0";
			bool multiple_short_arguments = true;
			for (int k = j; argv[i][k] != '\0'; k++) {
				char temp_string[2] = "\0";
				temp_string[0] = argv[i][k];
				if (command_line_settings_map.count(temp_string) == 0) {
					temp_alias[0] = argv[i][j];
					command_line_settings_map[temp_alias]->set_base_variable(argv[i] + 2);
					multiple_short_arguments = false;
					break;
				}
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
	list_of_cmd_var.clear();
	return non_options;
}

template<>
void Command_Line_Var<int>::set_base_variable(const char * b_v) {
	*(int *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<unsigned int>::set_base_variable(const char * b_v) {
	*(unsigned int *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<long>::set_base_variable(const char * b_v) {
	*(long *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<unsigned long>::set_base_variable(const char * b_v) {
	*(unsigned long *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<long long>::set_base_variable(const char * b_v) {
	*(long long *)base_variable = strtoll(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<unsigned long long>::set_base_variable(const char * b_v) {
	*(unsigned long long *)base_variable = strtoull(b_v, nullptr, 10);
}

template<>
void Command_Line_Var<float>::set_base_variable(const char * b_v) {
	*(float *)base_variable = strtof(b_v, nullptr);
}

template<>
void Command_Line_Var<double>::set_base_variable(const char * b_v) {
	*(double *)base_variable = strtod(b_v, nullptr);
}

template<>
void Command_Line_Var<long double>::set_base_variable(const char * b_v) {
	*(long double *)base_variable = strtold(b_v, nullptr);
}


