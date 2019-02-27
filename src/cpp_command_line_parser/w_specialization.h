#ifndef W_SPECIALIZATION_H
#define W_SPECIALIZATION_H
#include "args_parser_templates.h"
#include "hash_table.h"
#include <stdexcept>
#include <vector>

class W_INTERFACE;

// Named after the -W flag of gcc
class W_SPECIALIZATION {
friend class W_INTERFACE;
friend class Command_Line_Var<W_SPECIALIZATION>;
private:
	Hash_Table<W_INTERFACE> setters;
public:
	W_SPECIALIZATION(size_t initial_size) :	setters(Hash_Table<W_INTERFACE>(initial_size)) {}
	W_INTERFACE * operator[](const char * flag) {
		return setters[flag];
	}
};

class W_INTERFACE {
protected:
	void * base_variable;
public:
	W_INTERFACE(void * b_v, W_SPECIALIZATION & w_s, const char * alias) : base_variable(b_v) {
		w_s.setters.insert(alias, this);
	}
	virtual void set_base_variable(const char * flag) = 0;
};

template <typename T>
class W_VALUE : public W_INTERFACE {
private:
	T value;
public:
	W_VALUE(T & b_v, W_SPECIALIZATION & w_s, const char * flag, T v) : W_INTERFACE(&b_v, w_s, flag), value(v) {}
	virtual void set_base_variable(const char * flag) {
		if (flag[0] != '\0') {
			throw std::invalid_argument("Flag does not require arguments.");
		}
		*(T *)base_variable = value;
	}
};

template <typename T>
class W_ARG : public W_INTERFACE {
public:
	W_ARG(T & b_v, W_SPECIALIZATION & w_s, const char * alias) : W_INTERFACE(&b_v, w_s, alias) {}
	virtual void set_base_variable(const char * arg) {
		*(T *)base_variable = arg;
	}
};

template<>
class W_ARG<char> : public W_INTERFACE {
private:
	int buffer_size;
public:
	W_ARG(char * b_v, W_SPECIALIZATION & w_s, const char * alias, int b_s) : W_INTERFACE(b_v, w_s, alias), buffer_size(b_s) {}
	virtual void set_base_variable(const char * arg) {
		char * base_variable_string = (char *)base_variable;
		int i = 0;
		while (arg[i] != '\0' && i < buffer_size) {
			base_variable_string[i] = arg[i];
			i++;
		}
		base_variable_string[i] = '\0';
	}
};

template<>
inline void Command_Line_Var<W_SPECIALIZATION>::set_base_variable(const char * flag) {
	int split_location = 0;
	char temp_flag[1024];
	for (; flag[split_location] != '\0'; split_location++) {
		temp_flag[split_location] = flag[split_location];
		if (flag[split_location] == '=') {
			temp_flag[split_location] = '\0';
			split_location++;
			break;
		}
		temp_flag[split_location + 1] = '\0';
	}
	if (((W_SPECIALIZATION *)base_variable)->setters.count(temp_flag) == 0) {
		char error_message_buffer[] = "Option does not exist: -W\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		error_message_buffer[24] = aliases[0][0]; 
		for (int i = 0; i <= 32 && temp_flag[i] != '\0'; i++) {
			error_message_buffer[25 + i] = temp_flag[i];
		}
		throw std::invalid_argument(error_message_buffer);
	}
	((W_SPECIALIZATION *)base_variable)->setters[temp_flag]->set_base_variable(flag + split_location);
}

template<>
inline void W_ARG<int>::set_base_variable(const char * b_v) {
	*(int *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void W_ARG<unsigned int>::set_base_variable(const char * b_v) {
	*(unsigned int *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void W_ARG<long>::set_base_variable(const char * b_v) {
	*(long *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void W_ARG<unsigned long>::set_base_variable(const char * b_v) {
	*(unsigned long *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void W_ARG<long long>::set_base_variable(const char * b_v) {
	*(long long *)base_variable = strtoll(b_v, nullptr, 10);
}

template<>
inline void W_ARG<unsigned long long>::set_base_variable(const char * b_v) {
	*(unsigned long long *)base_variable = strtoull(b_v, nullptr, 10);
}

template<>
inline void W_ARG<float>::set_base_variable(const char * b_v) {
	*(float *)base_variable = strtof(b_v, nullptr);
}

template<>
inline void W_ARG<double>::set_base_variable(const char * b_v) {
	*(double *)base_variable = strtod(b_v, nullptr);
}

template<>
inline void W_ARG<long double>::set_base_variable(const char * b_v) {
	*(long double *)base_variable = strtold(b_v, nullptr);
}

#endif
