#include "treap.h"

namespace vm {
	bool Node::valid() const {
		if (bigger) {
			if (bigger->value <= value) { return false; }
			if (bigger->priority > priority) { return false; }
			if (! bigger->valid()) { return false; }
		}
		if (not_bigger) {
			if (not_bigger->value > value) { return false; }
			if (not_bigger->priority > priority) { return false; }
			if (! not_bigger->valid()) { return false; }
		}
		return true;
	}

	Node* Treap::insert(Node* node) {
		node->not_bigger = node->bigger = nullptr;
		std::uniform_int_distribution<std::size_t> dist {
			std::numeric_limits<std::size_t>::min(),
			std::numeric_limits<std::size_t>::max()
		};
		node->priority = dist(gen_);

		if (! root) { root = node; count = 1; return node; }
		auto orig { node };

		if (root->priority < node->priority) {
			std::swap(root, node);
		}

		Node* parent { root };
		for (;;) {
			if (node->value <= parent->value) {
				if (parent->not_bigger) {
					if (parent->not_bigger->priority < node->priority) {
						std::swap(parent->not_bigger, node);
					}
					parent = parent->not_bigger; continue;
				}
				parent->not_bigger = node;
				break;
			} else {
				if (parent->bigger) {
					if (parent->bigger->priority < node->priority) {
						std::swap(parent->bigger, node);
					}
					parent = parent->bigger; continue;
				}
				parent->bigger = node;
				break;
			}
		}
		++count; return orig;
	}

	Node* Treap::insert_at_root(Node* node) {
		if (! node) { return nullptr; }
		++count;
		if (! root) {
			node->value = node->priority = 0;
			node->not_bigger = node->bigger = nullptr;
			root = node;
			return node;
		}
		node->value = root->value;
		node->priority = root->priority;
		node->bigger = root->bigger;
		root->bigger = nullptr;
		node->not_bigger = root;
		root = node;
		return node;
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

	Node* Treap::extract_subtree_(Node* node) {
		Node* candidate;
		if (! node->bigger) {
			candidate = node->not_bigger;
		} else if (! node->not_bigger) {
			candidate = node->bigger;
		} else {
			if (node->bigger->priority > node->not_bigger->priority) {
				candidate = node->bigger;
				auto min { candidate };
				while (min->not_bigger) { min = min->not_bigger; }
				min->not_bigger = node->not_bigger;
			} else {
				candidate = node->not_bigger;
				auto max { candidate };
				while (max->bigger) { max = max->bigger; }
				max->bigger = node->bigger;
			}
		}
		node->not_bigger = node->bigger = nullptr;
		return candidate;
	}

	Node* Treap::erase(Node* node) {
		if (! node) { return nullptr; }

		Node* current { root }, *parent { nullptr };
		while (current && current != node) {
			parent = current;
			if (current->value < node->value) {
				current = current->bigger;
			} else {
				current = current->not_bigger;
			}
		}
		if (! current) { return nullptr; } // not in tree

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
}
