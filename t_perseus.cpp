#include "perseus.h"
#include "tree.h"

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
	vm::Node node;
	assert_(tree.insert(&node) == &node);
	assert_(tree.root == &node);
	assert_(tree.count == 1);
	assert_(! node.not_bigger);
	assert_(! node.bigger);
}

void triple_tree_insert() {
	vm::Tree tree;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
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
	vm::Node a, b, c;
	a.value = b.value = c.value = 1000;
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
	vm::Node a, b, c;
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
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
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
	vm::Node a;
	assert_(tree.erase(&a) == nullptr);
}

void erase_root_from_tree() {
	vm::Tree tree;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	tree.insert(&middle); tree.insert(&small); tree.insert(&big);
	assert_(tree.root == &middle);
	assert_(tree.erase(&middle) == &middle);
	assert_(tree.count == 2);
}

void erase_root() {
	vm::Tree tree;
	assert_(tree.erase_root() == nullptr);
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	tree.insert(&middle); tree.insert(&small); tree.insert(&big);
	assert_(tree.erase_root() == &middle);
	assert_(tree.erase_root() == &small);
	assert_(tree.erase_root() == &big);
	assert_(tree.erase_root() == nullptr);
}

void tree_count() {
	vm::Tree tree;
	assert_(tree.get(0) == nullptr);

	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
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
	assert_(pers.dirty());
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

void non_changing_write() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.set(1, 'b') == 'b');
	assert_(! pers.dirty());
}

static inline void tree_tests() {
	empty_tree();
	simple_tree_insert();
	triple_tree_insert();
	same_tree_insert();
	root_insert();
	tree_find();
	erase_from_empty_tree();
	erase_root_from_tree();
	erase_root();
	tree_count();
}

static inline void perseus_tests() {
	new_is_empty();
	simple_read();
	simple_write();
	write_persists();
	non_changing_write();
}

int main() {
	tree_tests();
	perseus_tests();

	std::cout << tests_ << " tests ok\n";
}

