#ifndef CPP_CMD_LINE_PARSER_TEMPLATES_H
#define CPP_CMD_LINE_PARSER_TEMPLATES_H
#include <vector>

namespace cli{
	
class CLI_Interface {
protected:
	bool takes_args_var;
	void * base_variable;
	std::vector<const char *> aliases;
	const char * help_message;
public:
	CLI_Interface(void * b_v, std::vector<const char *> a, bool ta, const char * hm = "");
	virtual ~CLI_Interface() = default;
	const std::vector<const char *>& get_aliases() const;
	bool takes_args() const;
	bool ignored() const;
	const char * get_help_message() const;
	virtual void set_base_variable(const char * b_v) = 0;
};

template<typename T>
class Var : public CLI_Interface {
public:
	Var(T & b_v, std::vector<const char *> a, bool ta, const char * hm = "");
	Var(T * b_v, std::vector<const char *> a, bool ta, const char * hm = "");
	virtual void set_base_variable(const char * b_v);
};

template<typename T>
class Vector : public CLI_Interface {
public:
	Vector(std::vector<T>& b_v, std::vector<const char *> a, const char * hm = "");
	Vector(std::vector<T>* b_v, std::vector<const char *> a, const char * hm = "");
	virtual void set_base_variable(const char * b_v);
};

template<typename T>
class Value : public CLI_Interface {
private:
	T value;
public:
	Value(T & b_v, std::vector<const char *> a, T v, const char * hm = "");
	Value(T * b_v, std::vector<const char *> a, T v, const char * hm = "");
	virtual void set_base_variable(const char * b_v);
};

template<>
class Var<char> : public CLI_Interface {
private:
	int buffer_size;
public:
	Var(char * b_v, std::vector<const char *> a, bool ta, int b_s, const char * hm = "");
	Var(char & b_v, std::vector<const char *> a, bool ta, const char * hm = "");
	virtual void set_base_variable(const char * b_v);
};

}

#endif
