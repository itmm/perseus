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
}

int main() {
	new_is_empty();
	simple_read();
}

