#pragma once

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
			using Pages = std::map<long, char[page_size]>;
			Pages clean_pages_;
			Pages dirty_pages_;

			std::random_device rnd_;
			std::mt19937 gen_ { rnd_() };

			Pages::iterator random_it(Pages& pages);
			void drop_some();
			void make_room();
			void write_page(long idx, char* page);
			char* get_page(long idx);
			char* get_dirty_page(long idx);

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
