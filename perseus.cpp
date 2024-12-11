#include "perseus.h"

namespace vm {
	void Perseus::write_page(long idx, char* page) {
		ios_.seekp(idx << page_bits);
		ios_.clear();
		ios_.write(page, page_size);
	}

	Perseus::Pages::iterator Perseus::random_it(Pages& pages) {
			std::uniform_int_distribution<size_t> dist { 0, pages.size() - 1 };
			auto it { pages.begin() };
			std::advance(it, dist(gen_));
			return it;
	}

	void Perseus::flush_some() {
		for (int i { some_count }; i; --i) {
			if (! dirty()) { break; }
			auto it { random_it(dirty_pages_) };
			write_page(it->first, it->second);
			memcpy(clean_pages_[it->first], it->second, page_size);
			dirty_pages_.erase(it);
		}
	}

	void Perseus::drop_some() {
		for (int i { some_count }; i; --i) {
			if (clean_pages_.empty()) { break; }
			auto it { random_it(clean_pages_) };
			clean_pages_.erase(it);
		}
	}

	void Perseus::flush() {
		while (dirty()) { flush_some(); }
	}

	size_t Perseus::pages() const {
		return clean_pages_.size() + dirty_pages_.size();
	}

	void Perseus::make_room() {
		if (pages() < page_count) { return; }
		while (dirty_pages_.size() > page_count/2) { flush_some(); }
		while (pages() >= page_count) { drop_some(); }
	}

	char* Perseus::get_page(long idx) {
		auto got { dirty_pages_.find(idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(idx);
		if (got != clean_pages_.end()) { return got->second; }
		make_room();
		ios_.seekg(idx << page_bits);
		char* buffer { clean_pages_[idx] };
		ios_.read(buffer, page_size);
		return buffer;
	}

	char* Perseus::get_dirty_page(long idx) {
		auto got { dirty_pages_.find(idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(idx);
		if (got != clean_pages_.end()) {
			memcpy(dirty_pages_[idx], got->second, page_size);
			clean_pages_.erase(got);
			return dirty_pages_[idx];
		}
		make_room();
		ios_.seekg(idx << page_bits);
		char* buffer { dirty_pages_[idx] };
		ios_.read(buffer, page_size);
		return buffer;
	}

	char Perseus::get(long address) {
		char* b { get_page(address >> page_bits) };
		return b[address & page_mask];
	}

	char Perseus::set(long address, char value) {
		char* b { get_dirty_page(address >> page_bits) };
		auto idx { address & page_mask };
		b[idx] = value;
		return b[idx];
	}

	bool Perseus::dirty() const {
		return ! dirty_pages_.empty();
   }
};

