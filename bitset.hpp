#include <cstring>

const size_t bitpos[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

namespace lib
{

	template <size_t SIZE>
	class bitset
	{
	private:
		size_t length, first;
		uint32_t *memory;
		
	public:
		bitset()
		{
			length = ((SIZE - 1) >> 5) + 1;
			memory = new uint32_t [length + 1]; // create an extra field to optimize for extraction
			memset(memory, 0x00, length << 2);
			first = 0;
		}
		
		inline uint32_t &operator [] (const size_t index) { return memory[index]; }
		inline bool test(const size_t x) { return memory[x >> 5] >> (x & 31) & 1; }
		
		inline void set() { memset(memory, 0xff, length << 2); first = length; }
		inline void set(const size_t x) { memory[x >> 5] |= 1 << (x & 31); }
		
		inline void reset() { memset(memory, 0x00, length << 2); first = 0; }
		inline void reset(const size_t x) { memory[x >> 5] &= ~(1 << (x & 31)); }
		
		// return smallest empty bit and set it
		int extract()
		{
			if (~memory[first] == 0)
				do { ++first; } while (~memory[first] == 0);
			if (first == length) return SIZE;
			
			size_t res = first << 5;
			uint32_t value = ~memory[first];
			
			// using de Bruijn multiplication to find the least signinficant '1' bit
			res += bitpos[((uint32_t)((value & -value) * 0x077CB531U)) >> 27];
				
			set(res);
			return res;
		}
		
		void operator = (const bitset &source)
		{
			first = source.first;
			memcpy(memory, source.memory, length << 2);
		}
	};

}
