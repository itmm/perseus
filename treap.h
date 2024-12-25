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
		bool is_bigger;
		std::size_t assert_valid() const;
	};

	struct Treap {
		Node* root;
		std::size_t count { 0 };

		explicit Treap(Node* root = nullptr): root { root } { }

		Node* insert(Node* node);
		Node* find(std::size_t value) const;
		Node* get(std::size_t position) const;
		Node* erase(Node* node);
		Node* erase_root() { return erase(root); }
		Node* erase_min();
		Node* erase_random();
		bool empty() const { return ! count; }
		void assert_valid() const { if (root) { root->assert_valid(); } }

	private:
		using Node_Or_Count_ = std::variant<Node*, std::size_t>;
		Node_Or_Count_ get_or_count_(std::size_t position, Node* start) const;
		Node* extract_subtree_(Node* node);

		Node* rotate_to_bigger_(Node* node, Node* parent);
		Node* rotate_to_not_bigger_(Node* node, Node* parent);

		std::random_device rnd_;
		std::mt19937 gen_ { rnd_() };
	};
}
