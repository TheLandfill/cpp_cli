#ifndef WSpecialization_H
#define WSpecialization_H
#include "args_parser_templates.h"
#include "hash_table.h"
#include <stdexcept>
#include <vector>
namespace cpp_cli{
class WInterface;

// Named after the -W flag of gcc
class WSpecialization {
friend class WInterface;
friend class Var<WSpecialization>;
private:
	Hash_Table<WInterface> setters;
public:
	WSpecialization(size_t initial_size) :	setters(Hash_Table<WInterface>(initial_size)) {}
	WInterface * operator[](const char * flag) {
		return setters[flag];
	}
};

class WInterface {
protected:
	void * base_variable;
public:
	WInterface(void * b_v, WSpecialization & w_s, const char * alias) : base_variable(b_v) {
		w_s.setters.insert(alias, this);
	}
	virtual void set_base_variable(const char * flag) = 0;
};

template <typename T>
class Wvalue : public WInterface {
private:
	T value;
public:
	Wvalue(T & b_v, WSpecialization & w_s, const char * flag, T v) : WInterface(&b_v, w_s, flag), value(v) {}
	virtual void set_base_variable(const char * flag) {
		if (flag[0] != '\0') {
			throw std::invalid_argument("Flag does not require arguments.");
		}
		*(T *)base_variable = value;
	}
};

template <typename T>
class Warg : public WInterface {
public:
	Warg(T & b_v, WSpecialization & w_s, const char * alias) : WInterface(&b_v, w_s, alias) {}
	virtual void set_base_variable(const char * arg) {
		*(T *)base_variable = arg;
	}
};

template<>
class Warg<char> : public WInterface {
private:
	int buffer_size;
public:
	Warg(char * b_v, WSpecialization & w_s, const char * alias, int b_s) : WInterface(b_v, w_s, alias), buffer_size(b_s) {}
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
inline void Var<WSpecialization>::set_base_variable(const char * flag) {
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
	if (((WSpecialization *)base_variable)->setters.count(temp_flag) == 0) {
		char error_message_buffer[] = "Option does not exist: -W\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
		error_message_buffer[24] = aliases[0][0]; 
		for (int i = 0; i <= 32 && temp_flag[i] != '\0'; i++) {
			error_message_buffer[25 + i] = temp_flag[i];
		}
		throw std::invalid_argument(error_message_buffer);
	}
	((WSpecialization *)base_variable)->setters[temp_flag]->set_base_variable(flag + split_location);
}

template<>
inline void Warg<int>::set_base_variable(const char * b_v) {
	*(int *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Warg<unsigned int>::set_base_variable(const char * b_v) {
	*(unsigned int *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Warg<long>::set_base_variable(const char * b_v) {
	*(long *)base_variable = strtol(b_v, nullptr, 10);
}

template<>
inline void Warg<unsigned long>::set_base_variable(const char * b_v) {
	*(unsigned long *)base_variable = strtoul(b_v, nullptr, 10);
}

template<>
inline void Warg<long long>::set_base_variable(const char * b_v) {
	*(long long *)base_variable = strtoll(b_v, nullptr, 10);
}

template<>
inline void Warg<unsigned long long>::set_base_variable(const char * b_v) {
	*(unsigned long long *)base_variable = strtoull(b_v, nullptr, 10);
}

template<>
inline void Warg<float>::set_base_variable(const char * b_v) {
	*(float *)base_variable = strtof(b_v, nullptr);
}

template<>
inline void Warg<double>::set_base_variable(const char * b_v) {
	*(double *)base_variable = strtod(b_v, nullptr);
}

template<>
inline void Warg<long double>::set_base_variable(const char * b_v) {
	*(long double *)base_variable = strtold(b_v, nullptr);
}
}
#endif
