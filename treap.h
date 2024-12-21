#pragma once

#include <cstddef>
#include <variant>
#include <random>

namespace vm {
	struct Node {
		Node* not_bigger;
		Node* bigger;
		std::size_t value;
		std::size_t priority;
		bool valid() const;
	};

	struct Treap {
		Node* root { nullptr };
		std::size_t count { 0 };

		Node* insert(Node* node);
		Node* insert_at_root(Node* node);
		Node* find(std::size_t value) const;
		Node* get(std::size_t position) const;
		Node* erase(Node* node);
		Node* erase_root() { return erase(root); }
		Node* erase_random();
		bool empty() const { return ! count; }
		bool valid() const { return ! root || root->valid(); }

	private:
		using Node_Or_Count_ = std::variant<Node*, std::size_t>;
		Node_Or_Count_ get_or_count_(std::size_t position, Node* start) const;
		Node* extract_subtree_(Node* node);

		std::random_device rnd_;
		std::mt19937 gen_ { rnd_() };
	};
}
