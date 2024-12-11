#include "perseus.h"

#include <cassert>
#include <sstream>

void new_is_empty() {
	std::stringstream ios;
	vm::Perseus pers { ios };
	assert(! pers.dirty());
}

void simple_read() {
	std::stringstream ios { "abc" };
	vm::Perseus pers { ios };
	assert(pers.get(1) == 'b');
	assert(pers.get(3) == 0);
}

void simple_write() {
	std::stringstream ios { "abc" };
	vm::Perseus pers { ios };
	assert(pers.set(1, 'x') == 'x');
	assert(pers.get(1) == 'x');
}

void write_persists() {
	std::stringstream ios { "abc" };
	{
		vm::Perseus pers { ios };
		assert(pers.set(1, 'x') == 'x');
	}
	assert(ios.str()[1] == 'x');
}

int main() {
	new_is_empty();
	simple_read();
	simple_write();
	write_persists();
}

