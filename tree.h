#pragma once

#include <cstddef>
#include <variant>

namespace vm {
	struct Node {
		Node* not_bigger;
		Node* bigger;
		size_t value;
	};

	struct Tree {
		Node* root { nullptr };
		size_t count { 0 };

		Node* insert(Node* node);
		Node* insert_at_root(Node* node);
		Node* find(size_t index) const;
		Node* get(size_t position) const;
		Node* erase(Node* node);
		Node* erase_root() { return erase(root); }

	private:
		using Node_Or_Count = std::variant<Node*, size_t>;
		Node_Or_Count get_or_count(size_t position, Node* start) const;
		Node* extract_subtree(Node* node);
	};
}
