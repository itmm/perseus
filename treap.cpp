#include "treap.h"

#include <cassert>
#include <iostream>

namespace vm {
	bool not_bigger_is_bigger(const Node* node) {
		if (! node->not_bigger) { return false; }
		if (! node->bigger) { return true; }
		return node->not_bigger->is_bigger && ! node->bigger->is_bigger;
	}

	bool bigger_is_bigger(const Node* node) {
		if (! node->bigger) { return false; }
		if (! node->not_bigger) { return true; }
		return node->bigger->is_bigger && ! node->not_bigger->is_bigger;
	}

	std::size_t Node::assert_valid() const {
		std::size_t bigger_count { 0 }, not_bigger_count { 0 };
		if (bigger) {
			assert(bigger->value >= value);
			bigger_count = bigger->assert_valid();
		}
		if (not_bigger) {
			assert(not_bigger->value <= value);
			not_bigger_count = not_bigger->assert_valid();
		}

		if (not_bigger_count < bigger_count) {
			assert(bigger_is_bigger(this));
			assert(! not_bigger_is_bigger(this));
		} else if (bigger_count < not_bigger_count) {
			assert(not_bigger_is_bigger(this));
			assert(! bigger_is_bigger(this));
		} else {
			assert(! bigger_is_bigger(this));
			assert(! not_bigger_is_bigger(this));
		}

		return 1 + bigger_count + not_bigger_count;
	}

	std::size_t Treap::count_nodes(Node* node) {
		if (! node) { return 0; };
		return 1 + count_nodes(node->not_bigger) + count_nodes(node->bigger);
	}

	Node* Treap::rotate_to_bigger_(Node* node, Node* parent) {
		assert(node);
		Node* not_bigger { node->not_bigger };
		assert(not_bigger);
		not_bigger->is_bigger = node->is_bigger; node->is_bigger = false;
		node->not_bigger = not_bigger->bigger;
		not_bigger->bigger = node;
		if (parent) {
			if (parent->bigger == node) {
				parent->bigger = not_bigger;
			} else {
				parent->not_bigger = not_bigger;
			}
		} else { root = not_bigger; }
		return not_bigger;
	}

	Node* Treap::rotate_to_not_bigger_(Node* node, Node* parent) {
		assert(node);
		Node* bigger { node->bigger };
		assert(bigger);
		bigger->is_bigger = node->is_bigger; node->is_bigger = false;
		node->bigger = bigger->not_bigger;
		bigger->not_bigger = node;
		if (parent) {
			if (parent->bigger == node) {
				parent->bigger = bigger;
			} else {
				parent->not_bigger = bigger;
			}
		} else { root = bigger; }
		return bigger;
	}

	void normalize(Node* node) {
		if (! node) { return; }
		if (! node->bigger || ! node->bigger->is_bigger) { return; }
		if (! node->not_bigger || ! node->not_bigger->is_bigger) { return; }
		node->bigger->is_bigger = node->not_bigger->is_bigger = false;
	}

	void make_not_bigger_bigger(Node* node) {
		assert(node && node->not_bigger);
		if (! node->bigger) {
			node->not_bigger->is_bigger = false;
		} else if (node->bigger->is_bigger) {
			node->bigger->is_bigger = node->not_bigger->is_bigger = false;
		} else {
			node->not_bigger->is_bigger = true;
		}
		normalize(node);
	}

	void make_not_bigger_smaller(Node* node) {
		assert(node && node->not_bigger);
		node->not_bigger->is_bigger = false;
		normalize(node);
	}

	void make_bigger_bigger(Node* node) {
		assert(node && node->bigger);
		if (! node->not_bigger) {
			node->bigger->is_bigger = false;
		} else if (node->not_bigger->is_bigger) {
			node->bigger->is_bigger = node->not_bigger->is_bigger = false;
		} else {
			node->bigger->is_bigger = true;
		}
		normalize(node);
	}

	Node* Treap::insert(Node* node) {
		node->not_bigger = node->bigger = nullptr; node->is_bigger = false;

		if (! root) { root = node; count = 1; return node; }

		Node* pre_parent { nullptr };
		Node* parent { root };
		for (;;) {
			if (node->value <= parent->value) {
				if (parent->not_bigger) {
					if (not_bigger_is_bigger(parent)) {
						parent = rotate_to_bigger_(parent, pre_parent);
					}
					if (parent->not_bigger) {
						make_not_bigger_bigger(parent);
						pre_parent = parent;
						parent = parent->not_bigger;
						continue;
					}
				}
				parent->not_bigger = node;
				break;
			} else {
				if (parent->bigger) {
					if (bigger_is_bigger(parent)) {
						parent = rotate_to_not_bigger_(parent, pre_parent);
					}
					if (parent->bigger) {
						make_bigger_bigger(parent);
						pre_parent = parent;
						parent = parent->bigger;
						continue;
					}
				}
				parent->bigger = node;
				break;
			}
		}
		++count; return node;
	}

	Node* Treap::find(std::size_t value) const {
		auto current { root };
		for (;;) {
			if (! current || current->value == value) { return current; }
			if (current->value < value) {
				current = current->bigger;
			} else {
				current = current->not_bigger;
			}
		}
	}

	inline Treap::Node_Or_Count_ Treap::get_or_count_(
		std::size_t position, Node* start
	) const {
		if (! start) { return static_cast<std::size_t>(0); }
		auto got { get_or_count_(position, start->not_bigger) };
		if (std::holds_alternative<Node*>(got)) { return got; }
		std::size_t not_bigger_count { std::get<std::size_t>(got) };
		position -= not_bigger_count;
		if (! position) { return start; }
		got = get_or_count_(position - 1, start->bigger);
		if (std::holds_alternative<Node*>(got)) { return got; }
		return not_bigger_count + 1 + std::get<std::size_t>(got);
	}

	Node* Treap::get(std::size_t position) const {
		auto got { get_or_count_(position, root) };
		if (std::holds_alternative<Node*>(got)) {
			return std::get<Node*>(got);
		} else {
			return nullptr;
		}
	}

	Node* Treap::erase_min() {
		if (! root) { return nullptr; }
		Node* current { root }, *parent { nullptr };
		while (current->not_bigger) { 
			if (bigger_is_bigger(current)) {
				current = rotate_to_not_bigger_(current, parent);
			}
			make_not_bigger_smaller(current);
			parent = current;
			current = current->not_bigger;
		}
		if (parent) {
			parent->not_bigger = current->bigger;
		} else {
			root = current->bigger;
		}
		current->bigger = nullptr;
		--count;
		return current;
	}

	Node* Treap::extract_subtree_(Node* node) {
		Node* candidate;
		if (! node->bigger) {
			candidate = node->not_bigger;
		} else if (! node->not_bigger) {
			candidate = node->bigger;
		} else {
			Treap not_bigger_tree { node->not_bigger };
			Treap bigger_tree { node->bigger };
			Treap merged;
			while (! not_bigger_tree.empty()) {
				merged.insert(not_bigger_tree.erase_min());
			}
			while (! bigger_tree.empty()) {
				merged.insert(bigger_tree.erase_min());
			}
			candidate = merged.root;
		}
		node->not_bigger = node->bigger = nullptr;
		return candidate;
	}

	Node* Treap::erase(Node* node) {
		if (! node) { return nullptr; }

		Node* current { root };
		while (current && current != node) {
			if (current->value < node->value) {
				current = current->bigger;
			} else {
				current = current->not_bigger;
			}
		}
		if (! current) { return nullptr; } // not in tree

		current = root;
		Node* parent { nullptr }, *pre_parent { nullptr };
		while (current && current != node) {
			parent = current;
			if (current->value < node->value) {
				if (not_bigger_is_bigger(parent)) {
					parent = rotate_to_bigger_(parent, pre_parent);
				}
				make_bigger_bigger(parent);
				pre_parent = parent;
				parent = parent->bigger;
				current = current->bigger;
			} else {
				if (bigger_is_bigger(parent)) {
					parent = rotate_to_not_bigger_(parent, pre_parent);
				}
				make_not_bigger_bigger(parent);
				pre_parent = parent;
				parent = parent->not_bigger;
				current = current->not_bigger;
			}
		}
		assert(current);

		current = extract_subtree_(node);

		if (! parent) {
			root = current;
		} else if (parent->bigger == node) {
			parent->bigger = current;
		} else {
			parent->not_bigger = current;
		}
		--count;
		return node;
	}

	Node* Treap::erase_random() {
		if (empty()) { return nullptr; }
		std::uniform_int_distribution<std::size_t> dist { 0, count - 1 };
		return erase(get(dist(gen_)));
	}

	void Treap::assert_valid() const {
		if (root) { 
			assert(root->assert_valid() == count);
		} else {
			assert(count == 0);
		}
	}
}
