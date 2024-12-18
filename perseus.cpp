#include "perseus.h"

namespace vm {
	Page* Tree::insert(Page* node) {
		node->not_bigger = node->bigger = nullptr;
		if (! root) { root = node; count = 1; return node; }
		Page* parent { root };
		for (;;) {
			if (node->index <= parent->index) {
				if (parent->not_bigger) { parent = parent->not_bigger; continue; }
				parent->not_bigger = node; break;
			} else {
				if (parent->bigger) { parent = parent->bigger; continue; }
				parent->bigger = node; break;
			}
		}
		++count; return node;
	}

	Page* Tree::insert_at_root(Page* node) {
		if (! node) { return nullptr; }
		if (! root) {
			node->index = 0;
			node->not_bigger = node->bigger = nullptr;
			root = node;
			return node;
		}
		node->index = root->index;
		node->bigger = root->bigger;
		root->bigger = nullptr;
		node->not_bigger = root;
		root = node;
		return node;
	}

	Page* Tree::find(size_t index) const {
		Page* current { root };
		for (;;) {
			if (! current || current->index == index) { return current; }
			if (current->index < index) {
				current = current->bigger;
			} else {
				current = current->not_bigger;
			}
		}
	}

	std::variant<Page*, size_t> Tree::get_or_count(
		size_t position, Page* start
	) const {
		if (! start) { return 0ul; }
		auto got { get_or_count(position, start->not_bigger) };
		if (std::holds_alternative<Page*>(got)) { return got; }
		size_t not_bigger_count { std::get<size_t>(got) };
		position -= not_bigger_count;
		if (! position) { return start; }
		got = get_or_count(position - 1, start->bigger);
		if (std::holds_alternative<Page*>(got)) { return got; }
		return not_bigger_count + 1 + std::get<size_t>(got);
	}

	Page* Tree::get(size_t position) const {
		auto got { get_or_count(position, root) };
		if (std::holds_alternative<Page*>(got)) {
			return std::get<Page*>(got);
		} else {
			return nullptr;
		}
	}

	Page* Tree::extract_subtree(Page* node) {
		Page* candidate;
		if (! node->bigger) {
			candidate = node->not_bigger;
			node->not_bigger = nullptr;
		} else if (! node->not_bigger) {
			candidate = node->bigger;
			node->bigger = nullptr;
		} else {
			candidate = node->not_bigger;
			Page* max { candidate };
			while (max->bigger) { max = max->bigger; }
			max->bigger = node->bigger;
			node->not_bigger = node->bigger = nullptr;
		}
		return candidate;
	}

	Page* Tree::erase(Page* node) {
		if (! node) { return nullptr; }

		Page* current { root }, *parent { nullptr };
		while (current && current != node) {
			parent = current;
			if (current->index < node->index) {
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

	void Perseus::write_page_(size_t idx, const Page& page) {
		ios_.seekp(idx << page_bits);
		ios_.clear();
		ios_.write(page.data, page_size);
	}

	Page& Perseus::random_page_(Tree& tree) {
		std::uniform_int_distribution<size_t> dist { 0, tree.count - 1 };
		return *tree.get(dist(gen_));
	}

	Perseus::Pages::iterator Perseus::random_it_(Pages& pages) {
		std::uniform_int_distribution<size_t> dist { 0, pages.size() - 1 };
		auto it { pages.begin() };
		std::advance(it, dist(gen_));
		return it;
	}

	void Perseus::flush_some() {
		for (int i { some_count }; i; --i) {
			if (! dirty()) { break; }
			auto it { random_it_(dirty_pages_) };
			write_page_(it->first, it->second);
			clean_pages_[it->first] = it->second;
			dirty_pages_.erase(it);
		}
	}

	void Perseus::drop_some_() {
		for (int i { some_count }; i; --i) {
			if (clean_pages_.empty()) { break; }
			auto it { random_it_(clean_pages_) };
			clean_pages_.erase(it);
		}
	}

	void Perseus::flush() {
		while (dirty()) { flush_some(); }
	}

	size_t Perseus::pages() const {
		return clean_pages_.size() + dirty_pages_.size();
	}

	void Perseus::make_room_() {
		if (pages() < page_count) { return; }
		while (dirty_pages_.size() > page_count/2) { flush_some(); }
		while (pages() >= page_count) { drop_some_(); }
	}

	Page& Perseus::get_page_(size_t idx) {
		auto got { dirty_pages_.find(idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(idx);
		if (got != clean_pages_.end()) { return got->second; }
		make_room_();
		ios_.seekg(idx << page_bits);
		auto& buffer { clean_pages_[idx] };
		ios_.read(buffer.data, page_size);
		return buffer;
	}

	Page& Perseus::get_dirty_page_(size_t idx) {
		auto got { dirty_pages_.find(idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(idx);
		if (got != clean_pages_.end()) {
			dirty_pages_[idx] = got->second;
			clean_pages_.erase(got);
			return dirty_pages_[idx];
		}
		make_room_();
		ios_.seekg(idx << page_bits);
		auto& buffer { dirty_pages_[idx] };
		ios_.read(buffer.data, page_size);
		return buffer;
	}

	char Perseus::get(size_t address) {
		auto& b { get_page_(address >> page_bits) };
		return b.data[address & page_mask];
	}

	char Perseus::set(size_t address, char value) {
		auto& b { get_dirty_page_(address >> page_bits) };
		auto idx { address & page_mask };
		b.data[idx] = value;
		return b.data[idx];
	}

	bool Perseus::dirty() const {
		return ! dirty_pages_.empty();
   }
};

