#ifndef COMMAND_LINE_VAR_HASH_TABLE_H
#define COMMAND_LINE_VAR_HASH_TABLE_H
#include <vector>
#include <cstddef>
#include <cstring>
#include <stdexcept>

// class Command_Line_Var_Interface;

template<typename T>
struct Hash_Table_Registration {
	const char * key;
	T * value;
	Hash_Table_Registration() : key(""), value(nullptr) {}
};

size_t find_next_prime(const size_t initial_guess);

// WARNING: THIS HASH TABLE ONLY WORKS FOR LESS THAN 5100 ELEMENTS
// UNLESS YOU INITIALIZE IT WITH A PRIME NUMBER SIZE AND DO NOT
// RESIZE IT. 5100 is 101^2 / 2. Also, this is defined specifically
// for command line argument parsing, so you should definitely have
// less than 5100 different aliases. As it stands, it does not
// have the capability to be resized.
template<typename T>
class Hash_Table {
private:
	size_t hash_multiple = 31;
	std::vector<Hash_Table_Registration<T>> registry_list;
	size_t num_elements = 0;
private:
	size_t hash_element(const char * str) const {
		if (strcmp(str, "") == 0) {
			return (size_t)-1;
		}
		size_t location = 7;
		int cur_char = 0;
		while (str[cur_char] != '\0') {
			location += str[cur_char];
			location *= hash_multiple;
			location %= registry_list.size();
			cur_char++;
		}
		while (strcmp(registry_list[location].key, "") != 0 && strcmp(registry_list[location].key, str) != 0) {
			location++;
			location %= registry_list.size();
		}
		return location;
	}

public:
	Hash_Table(size_t initial_size) {
		size_t actual_size = find_next_prime(2 * initial_size);
		if (actual_size != (size_t)-1) {
			registry_list = std::vector<Hash_Table_Registration<T> >(find_next_prime(2 * initial_size), Hash_Table_Registration<T>());
		} else {
			throw std::length_error("You are using more flags than this hash table can store. Please use fewer than 5000.");
		}
	}
	void insert(const char * str, T * val) {
		if (num_elements == registry_list.size()) {
			throw std::length_error("You have inserted more elements than the number that has been allocated for the hash table.");
		}
		size_t location = hash_element(str);
		registry_list[location].key = str;
		registry_list[location].value = val;
		num_elements++;
	}
	T * operator[](const char * str) const {
		return registry_list[hash_element(str)].value;
	}
	int count(const char * str) const {
		return strcmp(registry_list[hash_element(str)].key, "");
	}
};

#endif
