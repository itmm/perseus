#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <random>

namespace vm {
	static constexpr int page_bits { 12 };
	static constexpr int page_size { 1 << page_bits };
	static constexpr int page_mask { page_size - 1 };

	struct Page {
		char data[page_size];
		// char color;
		Page* not_bigger;
		Page* bigger;
		size_t index;
	};

	struct Tree {
		Page* root { nullptr };
		size_t count { 0 };

		Page* insert(Page* node);
		Page* find(size_t index) const;
	};

	class Perseus {
			std::iostream& ios_;
			Page* begin_;
			Page* end_;
			Page* top_;

			using Pages = std::map<size_t, Page>;
			Pages clean_pages_;
			Pages dirty_pages_;

			Tree free_;
			Tree clean_;
			Tree dirty_;

			std::random_device rnd_;
			std::mt19937 gen_ { rnd_() };

			Pages::iterator random_it_(Pages& pages);
			void drop_some_();
			void make_room_();
			void write_page_(size_t idx, const Page& page);
			Page& get_page_(size_t idx);
			Page& get_dirty_page_(size_t idx);

		public:
			static constexpr int page_count { 1024 };
			static constexpr int some_count { 10 };

			static_assert(page_bits < sizeof(size_t) * 8);
			static_assert(page_bits > 0);
			static_assert(page_count > 0);
			static_assert(some_count > 0);

			explicit Perseus(std::iostream& ios, Page* begin, Page* end):
				ios_ { ios }, begin_ { begin }, end_ { end }, top_ { begin_ }
			{
				assert(begin && end > begin);
			}

			~Perseus() { flush(); }

			void flush_some();
			void flush();

			char get(size_t address);
			char set(size_t address, char value);

			bool dirty() const;
			size_t pages() const;
	};
};
