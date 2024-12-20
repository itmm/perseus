#include "perseus.h"
#include "treap.h"

#include <cassert>
#include <sstream>

static int tests_ { 0 };

#define assert_(expr) do { ++tests_; assert(expr); } while (false)

void empty_tree() {
	vm::Treap treap;
	assert_(! treap.root);
	assert_(! treap.count);
	assert_(treap.valid());
}

void simple_tree_insert() {
	vm::Treap treap;
	vm::Node node;
	assert_(treap.insert(&node) == &node);
	assert_(treap.root == &node);
	assert_(treap.count == 1);
	assert_(treap.valid());
	assert_(! node.not_bigger);
	assert_(! node.bigger);
}

void triple_tree_insert() {
	vm::Treap treap;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	assert_(treap.insert(&middle) == &middle);
	assert_(treap.insert(&small) == &small);
	assert_(treap.insert(&big) == &big);
	assert_(treap.count == 3);
	assert_(treap.valid());
}

void same_tree_insert() {
	vm::Treap treap;
	vm::Node a, b, c;
	a.value = b.value = c.value = 1000;
	assert_(treap.insert(&a) == &a);
	assert_(treap.insert(&b) == &b);
	assert_(treap.insert(&c) == &c);
	assert_(treap.count == 3);
	assert_(treap.valid());
}

void root_insert() {
	vm::Treap treap;
	vm::Node a, b, c;
	assert_(treap.insert_at_root(&a) == &a);
	assert_(treap.root = &a);
	assert_(treap.insert_at_root(&b) == &b);
	assert_(treap.root = &b);
	assert_(treap.insert_at_root(&c) == &c);
	assert_(treap.root = &c);
	assert_(treap.count == 3);
	assert_(treap.valid());
}

void tree_find() {
	vm::Treap treap;
	assert_(treap.find(1000) == nullptr);
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle); treap.insert(&small); treap.insert(&big);
	assert_(treap.find(2000) == &middle);
	assert_(treap.find(1000) == &small);
	assert_(treap.find(3000) == &big);
	assert_(treap.find(999) == nullptr);
	assert_(treap.find(1500) == nullptr);
	assert_(treap.find(3001) == nullptr);
}

void erase_from_empty_tree() {
	vm::Treap treap;
	vm::Node a;
	assert_(treap.erase(&a) == nullptr);
}

void erase_root_from_tree() {
	vm::Treap treap;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle); treap.insert(&small); treap.insert(&big);
	assert_(treap.erase(&middle) == &middle);
	assert_(treap.count == 2);
	assert_(treap.valid());
}

void erase_root() {
	vm::Treap treap;
	assert_(treap.erase_root() == nullptr);
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle); treap.insert(&small); treap.insert(&big);
	assert_(treap.erase_root());
	assert_(treap.erase_root());
	assert_(treap.erase_root());
	assert_(treap.erase_root() == nullptr);
	assert_(treap.valid());
}

void tree_count() {
	vm::Treap treap;
	assert_(treap.get(0) == nullptr);

	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle);
	assert_(treap.get(0) == &middle);
	assert_(treap.get(1) == nullptr);

	treap.insert(&small); treap.insert(&big);
	assert_(treap.get(0) == &small);
	assert_(treap.get(1) == &middle);
	assert_(treap.get(2) == &big);
	assert_(treap.get(3) == nullptr);
	assert_(treap.valid());
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

void write_out_of_bounds() {
	std::stringstream ios { "abc" };
	{
		vm::Page pages[4];
		vm::Perseus pers { ios, pages, pages + 4 };
		assert_(pers.set(9000, 'c') == 'c');
	}
	assert_(ios.str().size() >= 9000);
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
	write_out_of_bounds();
}

int main() {
	tree_tests();
	perseus_tests();

	std::cout << tests_ << " tests ok\n";
}

