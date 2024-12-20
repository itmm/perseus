#pragma once

#include "tree.h"

#include <cassert>
#include <iostream>
#include <random>

namespace vm {
	static constexpr int page_bits { 12 };
	static constexpr int page_size { 1 << page_bits };
	static constexpr int page_mask { page_size - 1 };

	struct Page: public Node {
		char data[page_size];
	};

	class Perseus {
			std::iostream& ios_;

			Tree free_;
			Tree clean_;
			Tree dirty_;

			std::random_device rnd_;
			std::mt19937 gen_ { rnd_() };

			Page* random_page_(Tree& tree);

			void drop_some_();
			void make_room_();
			void write_page_(size_t position, const Page& page);
			std::pair<Page*, bool> get_page_(size_t position);
			Page& get_dirty_page_(size_t position);

		public:
			static constexpr int some_count { 10 };

			static_assert(page_bits < sizeof(size_t) * 8);
			static_assert(page_bits > 0);
			static_assert(some_count > 0);

			Perseus(std::iostream& ios, Page* begin, Page* end);
			~Perseus() { flush(); }

			void flush_some();
			void flush();

			char get(size_t address);
			char set(size_t address, char value);

			bool dirty() const;
			size_t pages() const;
	};
};
