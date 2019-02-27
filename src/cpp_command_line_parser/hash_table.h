#ifndef COMMAND_LINE_VAR_HASH_TABLE_H
#define COMMAND_LINE_VAR_HASH_TABLE_H
#include <vector>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>

// class Command_Line_Var_Interface;

template<typename T>
struct Hash_Table_Registration {
	const char * key;
	T * value;
	short probe_dist = 0;
	Hash_Table_Registration() : key(nullptr), value(nullptr) {}
};

// size_t find_next_prime(const size_t initial_guess);

template<typename T>
class Hash_Table {
private:
	static const size_t hash_multiple = 31;
	std::vector<Hash_Table_Registration<T>> registry_list;
	size_t num_elements = 0;
	double load_factor = 0.7;
	size_t table_size = 1;
	size_t num_tombstones = 0;
private:
	// A nullptr key is an untouched element
	// A "" is a tombstone
	size_t hash_element(const char * str) const {
		if (strcmp(str, "") == 0) {
			return (size_t)-1;
		}
		size_t hash = 7;
		int cur_char = 0;
		while (str[cur_char] != '\0') {
			hash += str[cur_char];
			hash *= hash_multiple;
			cur_char++;
		}
		hash &= table_size;
		return hash;
	}

	size_t find_element_beyond_tombstones(const char * str, size_t index) const {
		while (registry_list[index].key != nullptr && strcmp(registry_list[index].key, str) != 0) {
			index++;
			index &= table_size;
		}
		return index;
	}

public:
	Hash_Table(size_t initial_size) {
		while (table_size < (size_t)(-1) && initial_size > table_size * load_factor) {
			table_size <<= 1;
			table_size |= 1;
		}
		if (table_size < (size_t)(-1)) {
			registry_list = std::vector<Hash_Table_Registration<T> >(table_size + 1, Hash_Table_Registration<T>());
		} else {
			throw std::length_error("You are hashing more elements than a vector can hold.");
		}
	}

	void insert(const char * str, T * val) {
		size_t location = hash_element(str);
		if (location == (size_t)(-1)) {
			return;
		}
		short probe_dist = 0;
		while (registry_list[location].key != nullptr && strcmp(registry_list[location].key, "") != 0 && strcmp(registry_list[location].key, str) != 0) {
			location++;
			probe_dist++;
			Hash_Table_Registration<T> & cur_htr = registry_list[location];
			if (probe_dist < cur_htr.probe_dist) {
				const char * temp_str = str;
				str = cur_htr.key;
				cur_htr.key = temp_str;

				T * temp_val = val;
				val = cur_htr.value;
				cur_htr.value = temp_val;

				short temp_probe_dist = probe_dist;
				probe_dist = cur_htr.probe_dist;
				cur_htr.probe_dist = temp_probe_dist;
			}
			location &= table_size;
		}
		num_tombstones -= registry_list[location].key != nullptr;
		registry_list[location].key = str;
		registry_list[location].value = val;
		registry_list[location].probe_dist = probe_dist;
		num_elements++;
	}

	void remove(const char * str) {
		size_t location = hash_element(str);
		location = find_element_beyond_tombstones(str, location);
		if (registry_list[location].key == nullptr) {
			return;
		}
		num_elements--;
		while (registry_list[location].key != nullptr) {
			size_t optimal_element_to_swap = 0;
			for (size_t probe_improvement = 1; probe_improvement < 10 && registry_list[location + probe_improvement].key != nullptr; probe_improvement++) {
				if (registry_list[location + probe_improvement].probe_dist >= probe_improvement) {
					optimal_element_to_swap = probe_improvement;
				}
			}
			Hash_Table_Registration<T> & old = registry_list[location];
			Hash_Table_Registration<T> & cur = registry_list[location + optimal_element_to_swap];
			old.key = cur.key;
			old.value = cur.value;
			old.probe_dist = cur.probe_dist - optimal_element_to_swap;
			location += optimal_element_to_swap;
			location &= table_size;
		}
	}

	T * operator[](const char * str) const {
		size_t expected_location = find_element_beyond_tombstones(str, hash_element(str));
		if (strcmp(registry_list[expected_location].key, str) != 0) {
			char error_message_buffer[] = "Key not in table: \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
			for (int i = 0; i < 32 && str[i] != '\0'; i++) {
				error_message_buffer[i + 18] = str[i];
			}
			throw std::invalid_argument(error_message_buffer);
		}
		return registry_list[find_element_beyond_tombstones(str, hash_element(str))].value;
	}

	int count(const char * str) const {
		return registry_list[find_element_beyond_tombstones(str, hash_element(str))].key != nullptr;
	}

	void clear() {
		for (size_t i = 0; i < registry_list.size(); i++) {
			registry_list[i] = Hash_Table_Registration<T>();
		}
	}

	void rehash() {
		table_size <<= 1;
		table_size |= 1;
		if (table_size < (size_t)(-1)) {
			std::vector<Hash_Table_Registration<T> > new_hash_table(table_size + 1, Hash_Table_Registration<T>());
			registry_list.swap(new_hash_table);
			for (size_t i = 0; i < new_hash_table.size(); i++) {
				if (new_hash_table[i].key != nullptr && strcmp(new_hash_table[i].key, "") != 0) {
					insert(new_hash_table[i].key, new_hash_table[i].value);
				}
			}		
		} else {
			throw std::length_error("You are hashing more elements than a vector can hold.");
		}
	}
};

#endif
