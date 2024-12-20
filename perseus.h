#pragma once

#include "treap.h"

#include <iostream>

namespace vm {
	static constexpr int page_bits { 12 };
	static constexpr int page_size { 1 << page_bits };
	static constexpr int page_mask { page_size - 1 };

	struct Page: public Node {
		char data[page_size];
	};

	class Perseus {
			std::iostream& ios_;

			Treap free_;
			Treap clean_;
			Treap dirty_;

			void drop_some_();
			void make_room_();
			void write_page_(std::size_t position, const Page& page);
			std::pair<Page*, bool> get_page_(std::size_t position);

		public:
			static constexpr int some_count { 10 };

			static_assert(page_bits < sizeof(std::size_t) * 8);
			static_assert(page_bits > 0);
			static_assert(some_count > 0);

			Perseus(std::iostream& ios, Page* begin, Page* end);
			~Perseus() { flush(); }

			void flush_some();
			void flush();

			char get(std::size_t address);
			char set(std::size_t address, char value);

			bool dirty() const { return ! dirty_.empty(); }
			std::size_t pages() const { return clean_.count + dirty_.count; }
	};
};
