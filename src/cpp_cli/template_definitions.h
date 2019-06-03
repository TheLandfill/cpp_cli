#ifndef TEMPLATE_DEFINITIONS_H
#define TEMPLATE_DEFINITIONS_H
#include "args_parser_templates.h"
#include <cstdlib>
#include <stdexcept>

namespace cli {
/////////////////////CLI_Interface Definitions////////////////////

inline CLI_Interface::CLI_Interface(void * b_v, std::vector<const char *> a, bool ta, const char * hm) : takes_args_var(ta), base_variable(b_v), aliases(a), help_message(hm) {}

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
	const char * error_message = "Because the length of the char buffers in the vector cannot "
	"be specified and you cannot set a char * to a const char *, you cannot use char "
	"as an acceptable type for a Vector. Use const char *, std::string, or "
	"another template overload.";
	throw std::invalid_argument(error_message);
}

template<>
inline void Vector<char>::set_base_variable(const char * b_v) {
	std::vector<char>& base_variable_vector = *(std::vector<char>*)base_variable;
	while (*b_v != '\0') {
		base_variable_vector.push_back(*b_v);
		b_v++;
	}
}
}
#endif
