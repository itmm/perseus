#include "perseus.h"

namespace vm {
	void Perseus::flush_some() {
	}

	void Perseus::flush() {
		while (dirty()) { flush_some(); }
	}

	void Perseus::make_room() { }

	char* Perseus::get_page(long page_idx) {
		auto got { dirty_pages_.find(page_idx) };
		if (got != dirty_pages_.end()) { return got->second; }
		got = clean_pages_.find(page_idx);
		if (got != clean_pages_.end()) { return got->second; }
		make_room();
		ios_.seekg(page_idx << page_bits);
		char* buffer { clean_pages_[page_idx] };
		ios_.read(buffer, page_size);
		return buffer;
	}

	char Perseus::get(long address) {
		char* b { get_page(address >> page_bits) };
		return b[address & page_mask];
	}

	void Perseus::set(long address, char value) { }

	bool Perseus::dirty() const {
		return ! dirty_pages_.empty();
   }
};

