#include "tree.h"

namespace vm {
	Node* Tree::insert(Node* node) {
		node->not_bigger = node->bigger = nullptr;
		if (! root) { root = node; count = 1; return node; }
		auto parent { root };
		for (;;) {
			if (node->value <= parent->value) {
				if (parent->not_bigger) { parent = parent->not_bigger; continue; }
				parent->not_bigger = node; break;
			} else {
				if (parent->bigger) { parent = parent->bigger; continue; }
				parent->bigger = node; break;
			}
		}
		++count; return node;
	}

	Node* Tree::insert_at_root(Node* node) {
		if (! node) { return nullptr; }
		if (! root) {
			node->value = 0;
			node->not_bigger = node->bigger = nullptr;
			root = node;
			return node;
		}
		node->value = root->value;
		node->bigger = root->bigger;
		root->bigger = nullptr;
		node->not_bigger = root;
		root = node;
		return node;
	}

	Node* Tree::find(std::size_t value) const {
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

	Tree::Node_Or_Count Tree::get_or_count(std::size_t position, Node* start) const {
		if (! start) { return 0ul; }
		auto got { get_or_count(position, start->not_bigger) };
		if (std::holds_alternative<Node*>(got)) { return got; }
		std::size_t not_bigger_count { std::get<std::size_t>(got) };
		position -= not_bigger_count;
		if (! position) { return start; }
		got = get_or_count(position - 1, start->bigger);
		if (std::holds_alternative<Node*>(got)) { return got; }
		return not_bigger_count + 1 + std::get<std::size_t>(got);
	}

	Node* Tree::get(std::size_t position) const {
		auto got { get_or_count(position, root) };
		if (std::holds_alternative<Node*>(got)) {
			return std::get<Node*>(got);
		} else {
			return nullptr;
		}
	}

	Node* Tree::extract_subtree(Node* node) {
		Node* candidate;
		if (! node->bigger) {
			candidate = node->not_bigger;
			node->not_bigger = nullptr;
		} else if (! node->not_bigger) {
			candidate = node->bigger;
			node->bigger = nullptr;
		} else {
			candidate = node->not_bigger;
			auto max { candidate };
			while (max->bigger) { max = max->bigger; }
			max->bigger = node->bigger;
			node->not_bigger = node->bigger = nullptr;
		}
		return candidate;
	}

	Node* Tree::erase(Node* node) {
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

		current = extract_subtree(node);

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
}
