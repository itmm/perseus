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
		auto start { position << page_bits };
		ios_.seekp(start);
		if (! ios_) {
			ios_.clear();
			ios_.seekp(0, std::ios_base::end);
			auto end { ios_.tellp() };
			if (end >= start) { return; }
			char empty_page[page_size] { 0 };
			while (start - end > page_size) {
				ios_.write(empty_page, page_size);
				end += page_size;
			}
			if (start > end) {
				ios_.write(empty_page, start - end);
			}
		}
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
};

