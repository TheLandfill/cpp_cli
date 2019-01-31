#ifndef COMMAND_LINE_VAR_HASH_TABLE_H
#define COMMAND_LINE_VAR_HASH_TABLE_H
#include <vector>
#include <cstddef>

class Command_Line_Var_Interface;

struct Hash_Table_Registration {
	const char * key;
	Command_Line_Var_Interface * value;
	Hash_Table_Registration();
};

// WARNING: THIS HASH TABLE ONLY WORKS FOR LESS THAN 5100 ELEMENTS
// UNLESS YOU INITIALIZE IT WITH A PRIME NUMBER SIZE AND DO NOT
// RESIZE IT. 5100 is 101^2 / 2. Also, this is defined specifically
// for command line argument parsing, so you should definitely have
// less than 5100 different aliases.
class Hash_Table {
private:
	size_t hash_multiple = 31;
	std::vector<Hash_Table_Registration> registry_list;
	size_t hash_element(const char * str) const;
public:
	Hash_Table(size_t initial_size);
	void insert(const char * str, Command_Line_Var_Interface * clvi);
	Command_Line_Var_Interface * operator[](const char * str) const;
	int count(const char * str) const;
};

#endif
