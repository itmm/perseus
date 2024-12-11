#pragma once

#include <iostream>
#include <map>

namespace vm {
	class Perseus {
		public:
			static constexpr int page_bits { 12 };
			static constexpr int page_size { 1 << page_bits };
			static constexpr int page_mask { page_size - 1 };

		private:
			std::iostream& ios_;
			std::map<long, char[page_size]> clean_pages_;
			std::map<long, char[page_size]> dirty_pages_;

			void make_room();
			char* get_page(long page_idx);

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
			void set(long address, char value);

			bool dirty() const;
	};
};
