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
	assert_(! node.not_bigger);
	assert_(! node.bigger);
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
	assert_(middle.not_bigger == &small);
	assert_(middle.bigger == &big);
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
	assert_(a.not_bigger == &b);
	assert_(b.not_bigger == &c);
}

void root_insert() {
	vm::Tree tree;
	vm::Page a, b, c;
	assert_(tree.insert_at_root(&a) == &a);
	assert_(tree.root = &a);
	assert_(tree.insert_at_root(&b) == &b);
	assert_(tree.root = &b);
	assert_(tree.insert_at_root(&c) == &c);
	assert_(tree.root = &c);
}

void tree_find() {
	vm::Tree tree;
	assert_(tree.find(1000) == nullptr);
	vm::Page small, middle, big;
	small.index = 1000;
	middle.index = 2000;
	big.index = 3000;
	tree.insert(&middle); tree.insert(&small); tree.insert(&big);
	assert_(tree.find(2000) == &middle);
	assert_(tree.find(1000) == &small);
	assert_(tree.find(3000) == &big);
	assert_(tree.find(999) == nullptr);
	assert_(tree.find(1500) == nullptr);
	assert_(tree.find(3001) == nullptr);
}

void erase_from_empty_tree() {
	vm::Tree tree;
	vm::Page a;
	assert_(tree.erase(&a) == nullptr);
}

void erase_root_from_tree() {
	vm::Tree tree;
	vm::Page small, middle, big;
	small.index = 1000;
	middle.index = 2000;
	big.index = 3000;
	tree.insert(&middle); tree.insert(&small); tree.insert(&big);
	assert_(tree.root == &middle);
	assert_(tree.erase(&middle) == &middle);
	assert_(tree.count == 2);
}

void erase_any() {
	vm::Tree tree;
	assert_(tree.erase() == nullptr);
	vm::Page small, middle, big;
	small.index = 1000;
	middle.index = 2000;
	big.index = 3000;
	tree.insert(&middle); tree.insert(&small); tree.insert(&big);
	assert_(tree.erase() == &middle);
	assert_(tree.erase() == &small);
	assert_(tree.erase() == &big);
	assert_(tree.erase() == nullptr);
}

void tree_count() {
	vm::Tree tree;
	assert_(tree.get(0) == nullptr);

	vm::Page small, middle, big;
	small.index = 1000;
	middle.index = 2000;
	big.index = 3000;
	tree.insert(&middle);
	assert_(tree.get(0) == &middle);
	assert_(tree.get(1) == nullptr);

	tree.insert(&small); tree.insert(&big);
	assert_(tree.get(0) == &small);
	assert_(tree.get(1) == &middle);
	assert_(tree.get(2) == &big);
	assert_(tree.get(3) == nullptr);
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
	root_insert();
	tree_find();
	erase_from_empty_tree();
	erase_root_from_tree();
	erase_any();
	tree_count();
	new_is_empty();
	simple_read();
	simple_write();
	write_persists();
	std::cout << tests_ << " tests ok\n";
}

