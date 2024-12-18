#include "perseus.h"

#include <cassert>
#include <sstream>

static int tests_ { 0 };

#define assert_(expr) do { ++tests_; assert(expr); } while (false)

void empty_tree() {
	vm::Tree tree;
	assert_(! tree.root);
	assert_(! tree.count);
}

void simple_tree_insert() {
	vm::Tree tree;
	vm::Page node;
	assert_(tree.insert(&node) == &node);
	assert_(tree.root == &node);
	assert_(tree.count == 1);
	assert_(! node.left);
	assert_(! node.right);
}

void triple_tree_insert() {
	vm::Tree tree;
	vm::Page small, middle, big;
	small.index = 1000;
	middle.index = 2000;
	big.index = 3000;
	assert_(tree.insert(&middle) == &middle);
	assert_(tree.insert(&small) == &small);
	assert_(tree.insert(&big) == &big);
	assert_(tree.count == 3);
	assert_(tree.root == &middle);
	assert_(middle.left == &small);
	assert_(middle.right == &big);
}

void same_tree_insert() {
	vm::Tree tree;
	vm::Page a, b, c;
	a.index = b.index = c.index = 1000;
	assert_(tree.insert(&a) == &a);
	assert_(tree.insert(&b) == &b);
	assert_(tree.insert(&c) == &c);
	assert_(tree.count == 3);
	assert_(tree.root == &a);
	assert_(a.left == &b);
	assert_(b.left == &c);
}

void new_is_empty() {
	std::stringstream ios;
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(! pers.dirty());
}

void simple_read() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.get(1) == 'b');
	assert_(pers.get(3) == 0);
}

void simple_write() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.set(1, 'x') == 'x');
	assert_(pers.get(1) == 'x');
}

void write_persists() {
	std::stringstream ios { "abc" };
	{
		vm::Page pages[4];
		vm::Perseus pers { ios, pages, pages + 4 };
		assert_(pers.set(1, 'x') == 'x');
	}
	assert_(ios.str()[1] == 'x');
}

int main() {
	empty_tree();
	simple_tree_insert();
	triple_tree_insert();
	same_tree_insert();
	new_is_empty();
	simple_read();
	simple_write();
	write_persists();
	std::cout << tests_ << " tests ok\n";
}

