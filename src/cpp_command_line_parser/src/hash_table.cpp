#include "hash_table.h"

size_t find_next_prime(const size_t initial_guess) {
	if (initial_guess > 101 * 101 / 2) {
		return (size_t)-1;
	}
	size_t next_prime = initial_guess;
	next_prime += (next_prime & 1) ^ 1; // += 1 if even, += 0 if odd.
	bool is_prime = false;
	std::vector<size_t> list_of_primes = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97 };
	while (!is_prime) {
		is_prime = true;
		for (size_t i = 0; i < list_of_primes.size() && next_prime < list_of_primes[i] * list_of_primes[i]; i++) {
			size_t remainder = next_prime % list_of_primes[i];
			is_prime = is_prime && (remainder != 0);
			next_prime += 2 * (remainder == 0);
		}
	}
	return next_prime;
}
