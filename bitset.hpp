#include <cstring>

static const int bitpos[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

namespace usr
{

template <int SIZE>
class bitset
{
private:
	int length, first;
	uint32_t *memory;
	
public:
	bitset()
	{
		length = ((SIZE - 1) >> 5) + 1;
		memory = new uint32_t [length];
		memset(memory, 0x00, length << 2);
		first = 0;
	}
	
	inline bool test(const int x)
	{
		return memory[x >> 5] >> (x & 31) & 1;
	}
	
	inline void set(const int x)
	{
		memory[x >> 5] |= 1 << (x & 31);
	}
	
	// return smallest empty bit and set it
	int extract()
	{
		while (first < length && ~memory[first] == 0) ++first;
		if (first == length) return length << 5;
		
		int res = first << 5;
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
