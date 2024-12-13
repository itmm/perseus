#include "perseus.h"

namespace vm {
	void Perseus::write_page_(long idx, const Page& page) {
		ios_.seekp(idx << page_bits);
		ios_.clear();
		ios_.write(page.data(), page.size());
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

	Perseus::Page& Perseus::get_page_(long idx) {
		auto got { dirty_pages_.find(idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(idx);
		if (got != clean_pages_.end()) { return got->second; }
		make_room_();
		ios_.seekg(idx << page_bits);
		auto& buffer { clean_pages_[idx] };
		ios_.read(buffer.data(), buffer.size());
		return buffer;
	}

	Perseus::Page& Perseus::get_dirty_page_(long idx) {
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
		ios_.read(buffer.data(), buffer.size());
		return buffer;
	}

	char Perseus::get(long address) {
		auto& b { get_page_(address >> page_bits) };
		return b[address & page_mask];
	}

	char Perseus::set(long address, char value) {
		auto& b { get_dirty_page_(address >> page_bits) };
		auto idx { address & page_mask };
		b[idx] = value;
		return b[idx];
	}

	bool Perseus::dirty() const {
		return ! dirty_pages_.empty();
   }
};

