#include "perseus.h"
#include "treap.h"

#include <cassert>
#include <sstream>

static int tests_ { 0 };

#define assert_(expr) do { ++tests_; assert(expr); } while (false)

static inline void empty_tree() {
	vm::Treap treap;
	assert_(! treap.root);
	assert_(! treap.count);
	treap.assert_valid();
}

static inline void simple_tree_insert() {
	vm::Treap treap;
	vm::Node node;
	assert_(treap.insert(&node) == &node);
	assert_(treap.root == &node);
	assert_(treap.count == 1);
	treap.assert_valid();
	assert_(! node.not_bigger);
	assert_(! node.bigger);
}

static inline void triple_tree_insert() {
	vm::Treap treap;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	assert_(treap.insert(&middle) == &middle);
	assert_(treap.insert(&small) == &small);
	assert_(treap.insert(&big) == &big);
	assert_(treap.count == 3);
	treap.assert_valid();
}

static inline void same_tree_insert() {
	vm::Treap treap;
	vm::Node a, b, c;
	a.value = b.value = c.value = 1000;
	assert_(treap.insert(&a) == &a);
	assert_(treap.insert(&b) == &b);
	assert_(treap.insert(&c) == &c);
	assert_(treap.count == 3);
	treap.assert_valid();
}

static inline void tree_find() {
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

static inline void erase_from_empty_tree() {
	vm::Treap treap;
	vm::Node a;
	assert_(treap.erase(&a) == nullptr);
}

static inline void erase_root_from_tree() {
	vm::Treap treap;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle); treap.insert(&small); treap.insert(&big);
	assert_(treap.erase(&middle) == &middle);
	assert_(treap.count == 2);
	treap.assert_valid();
}

static inline void erase_min_from_tree() {
	vm::Treap treap;
	vm::Node small, middle, big;
	small.value = 1000;
	middle.value = 2000;
	big.value = 3000;
	treap.insert(&middle); treap.insert(&small); treap.insert(&big);
	assert_(treap.erase_min() == &small);
	assert_(treap.erase_min() == &middle);
	assert_(treap.erase_min() == &big);
	assert_(treap.empty());
}

static inline void erase_root() {
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
	treap.assert_valid();
}

static inline void tree_count() {
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
	treap.assert_valid();
}

static inline void new_is_empty() {
	std::stringstream ios;
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(! pers.dirty());
}

static inline void simple_read() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.get(1) == 'b');
	assert_(pers.get(3) == 0);
}

static inline void simple_write() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.set(1, 'x') == 'x');
	assert_(pers.get(1) == 'x');
	assert_(pers.dirty());
}

static inline void write_persists() {
	std::stringstream ios { "abc" };
	{
		vm::Page pages[4];
		vm::Perseus pers { ios, pages, pages + 4 };
		assert_(pers.set(1, 'x') == 'x');
	}
	assert_(ios.str()[1] == 'x');
}

static inline void non_changing_write() {
	std::stringstream ios { "abc" };
	vm::Page pages[4];
	vm::Perseus pers { ios, pages, pages + 4 };
	assert_(pers.set(1, 'b') == 'b');
	assert_(! pers.dirty());
}

static inline void write_out_of_bounds() {
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
	tree_find();
	erase_from_empty_tree();
	erase_root_from_tree();
	erase_min_from_tree();
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

