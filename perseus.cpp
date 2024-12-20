#include "perseus.h"

namespace vm {
	Perseus::Perseus(std::iostream& ios, Page* begin, Page* end):
		ios_ { ios }
	{
		if (begin) {
			for (; begin < end; ++begin) { free_.insert_at_root(begin); }
		}
	}

	void Perseus::write_page_(std::size_t position, const Page& page) {
		ios_.seekp(position << page_bits);
		ios_.clear();
		ios_.write(page.data, page_size);
	}

	void Perseus::flush_some() {
		for (int i { some_count }; i; --i) {
			if (! dirty()) { break; }
			auto page { static_cast<Page*>(dirty_.erase_random()) };
			if (! page) { break; }
			write_page_(page->value, *page);
			clean_.insert(page);
		}
	}

	void Perseus::drop_some_() {
		for (int i { some_count }; i; --i) {
			if (clean_.empty()) { break; }
			auto page { clean_.erase_random() };
			free_.insert(page);
		}
	}

	void Perseus::flush() {
		while (dirty()) { flush_some(); }
	}

	std::size_t Perseus::pages() const {
		return clean_.count + dirty_.count;
	}

	void Perseus::make_room_() {
		if (! free_.empty()) { return; }
		if (dirty_.count >= clean_.count) {
			flush_some();
		} else {
			drop_some_();
		}
	}

	std::pair<Page*, bool> Perseus::get_page_(std::size_t position) {
		auto got { static_cast<Page*>(dirty_.find(position)) };
		if (got) { return { got, true }; }
		got = static_cast<Page*>(clean_.find(position));
		if (got) { return { got, false }; }
		make_room_();
		got = static_cast<Page*>(free_.erase_root());
		got->value = position;
		clean_.insert(got);
		ios_.seekg(position << page_bits);
		ios_.read(got->data, page_size);
		if (! ios_) {
			auto written { ios_.gcount() };
			std::fill(got->data + written, got->data + page_size, 0);
			ios_.clear();
		}
		return { got, false };
	}

	char Perseus::get(std::size_t address) {
		auto got { get_page_(address >> page_bits) };
		return got.first->data[address & page_mask];
	}

	char Perseus::set(std::size_t address, char value) {
		auto got { get_page_(address >> page_bits) };
		auto index { address & page_mask };
		if (got.first->data[index] == value) {
			return got.first->data[index];
		}
		if (! got.second) { 
			clean_.erase(got.first);
			dirty_.insert(got.first);
		}
		return got.first->data[index] = value;
	}

	bool Perseus::dirty() const {
		return ! dirty_.empty();
   }
};

