#pragma once

#include <array>
#include <iostream>
#include <map>
#include <random>

namespace vm {
	class Perseus {
		public:
			static constexpr int page_bits { 12 };
			static constexpr int page_size { 1 << page_bits };
			static constexpr int page_mask { page_size - 1 };

		private:
			std::iostream& ios_;
			using Page = std::array<char, page_size>;
			using Pages = std::map<long, Page>;
			Pages clean_pages_;
			Pages dirty_pages_;

			std::random_device rnd_;
			std::mt19937 gen_ { rnd_() };

			Pages::iterator random_it_(Pages& pages);
			void drop_some_();
			void make_room_();
			void write_page_(long idx, const Page& page);
			Page& get_page_(long idx);
			Page& get_dirty_page_(long idx);

		public:
			static constexpr int page_count { 1024 };
			static constexpr int some_count { 10 };

			static_assert(page_bits < sizeof(long) * 8);
			static_assert(page_bits > 0);
			static_assert(page_count > 0);
			static_assert(some_count > 0);

			explicit Perseus(std::iostream& ios): ios_ { ios } { }

			~Perseus() { flush(); }

			void flush_some();
			void flush();

			char get(long address);
			char set(long address, char value);

			bool dirty() const;
			size_t pages() const;
	};
};
