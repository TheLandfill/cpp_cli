#ifndef CPP_CMD_LINE_PARSER_TEMPLATES_H
#define CPP_CMD_LINE_PARSER_TEMPLATES_H
#include <vector>

class Command_Line_Var_Interface {
protected:
	bool takes_args_var;
	void * base_variable;
	std::vector<const char *> aliases;
public:
	Command_Line_Var_Interface(void * b_v, std::vector<const char *> a, bool ta);
	const std::vector<const char *>& get_aliases() const;
	bool takes_args() const;
	bool ignored() const;
	virtual void set_base_variable(const char * b_v) = 0;
};

template<typename T>
class Command_Line_Var : public Command_Line_Var_Interface {
public:
	Command_Line_Var(T * b_v, std::vector<const char *> a, bool ta);
	virtual void set_base_variable(const char * b_v);
};

template<>
class Command_Line_Var<char> : public Command_Line_Var_Interface {
private:
	int buffer_size;
public:
	Command_Line_Var(void * b_v, std::vector<const char *> a, bool ta, int b_s);
	virtual void set_base_variable(const char * b_v);
};



#endif
