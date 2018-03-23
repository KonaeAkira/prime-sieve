#include <cstring>

namespace usr
{

template <int SIZE>
class bitset
{
private:
	int length, *memory, first;
	
public:
	bitset()
	{
		length = ((SIZE - 1) >> 5) + 1;
		memory = new int [length];
		memset(memory, 0x00, length << 2);
		first = 0;
	}
	
	inline bool test(register const int x)
	{
		return memory[x >> 5] >> (x & 31) & 1;
	}
	
	inline void set(register const int x)
	{
		memory[x >> 5] |= 1 << (x & 31);
	}
	
	// return smallest empty bit and set it
	int extract()
	{
		while (first < length && ~memory[first] == 0) ++first;
		if (first == length) return length << 5;
		
		register int res = ((first + 1) << 5) - 1;
		register int value = ~memory[first];
		
		if (value << 16 != 0) { value <<= 16; res -= 16; }
		if (value << 8 != 0) { value <<= 8; res -= 8; }
		if (value << 4 != 0) { value <<= 4; res -= 4; }
		if (value << 2 != 0) { value <<= 2; res -= 2; }
		if (value << 1 != 0) { value <<= 1; res -= 1; }
			
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
