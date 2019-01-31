#ifndef CPP_CMD_LINE_PARSER_H
#define CPP_CMD_LINE_PARSER_H
#include <cstddef>
#include <vector>

class Command_Line_Var_Interface {
protected:
	bool takes_args_var;
	void * base_variable;
	std::vector<const char *> aliases;
public:
	Command_Line_Var_Interface(void * b_v, std::vector<const char *> a, bool ta);
	const std::vector<const char *>& get_aliases() const {
		return aliases;
	}
	bool takes_args();
	virtual void set_base_variable(const char * b_v) = 0;
};

template<typename T>
class Command_Line_Var : public Command_Line_Var_Interface {
public:
	Command_Line_Var(T * b_v, std::vector<const char *> a, bool ta) : Command_Line_Var_Interface(b_v, a, ta) {}
	virtual void set_base_variable(const char * b_v) {
		*(T *)base_variable = b_v;
	}
};

template<typename T>
struct Command_Line_Var_Holder {
private:
	static std::vector<Command_Line_Var<T>> command_line_var_holder;
public:
	Command_Line_Var_Interface * add_to_command_line_var_holder(T * var_addr, std::vector<const char *> aliases, bool ta) {
		command_line_var_holder.emplace_back(var_addr, aliases, ta);
		return &(command_line_var_holder.back());
	}
};

std::vector<const char *> hash_cmd_line_into_variables(int argc, char ** argv, size_t num_unique_flags = 1000);

template<>
void Command_Line_Var<char>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<int>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<unsigned int>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<long>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<unsigned long>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<long long>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<unsigned long long>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<float>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<double>::set_base_variable(const char * b_v);

template<>
void Command_Line_Var<long double>::set_base_variable(const char * b_v);
#endif
