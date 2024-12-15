#include "perseus.h"

#include <cassert>
#include <sstream>

void empty_tree() {
	vm::Tree tree;
	assert(! tree.root);
	assert(! tree.count);
}

void simple_tree_insert() {
	vm::Tree tree;
	vm::Page node;
	assert(tree.insert(&node) == &node);
	assert(tree.root == &node);
	assert(tree.count == 1);
	assert(! node.left);
	assert(! node.right);
}

void new_is_empty() {
	std::stringstream ios;
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert(! pers.dirty());
}

void simple_read() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert(pers.get(1) == 'b');
	assert(pers.get(3) == 0);
}

void simple_write() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert(pers.set(1, 'x') == 'x');
	assert(pers.get(1) == 'x');
}

void write_persists() {
	std::stringstream ios { "abc" };
	{
		vm::Page pages[4];
		vm::Perseus pers { ios, pages, pages + 4 };
		assert(pers.set(1, 'x') == 'x');
	}
	assert(ios.str()[1] == 'x');
}

int main() {
	empty_tree();
	simple_tree_insert();
	new_is_empty();
	simple_read();
	simple_write();
	write_persists();
}

