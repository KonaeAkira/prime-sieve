#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <algorithm>

class number
{
private:
	static constexpr uint8_t value[8] = {1, 7, 11, 13, 17, 19, 23, 29};
	
	static constexpr uint8_t key[30] =
	{
		0, 0,
		1, 1, 1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3,
		4, 4, 4, 4,
		5, 5,
		6, 6, 6, 6,
		7, 7, 7, 7, 7, 7
	};
	
	static constexpr uint8_t times[8][8] =
	{
		{ 6, 4, 2, 4, 6, 4, 2, 2 },
		{ 4, 6, 2, 4, 2, 4, 2, 6 },
		{ 2, 6, 6, 2, 4, 2, 4, 4 },
		{ 4, 4, 6, 6, 2, 4, 2, 2 },
		{ 2, 2, 4, 2, 6, 6, 4, 4 },
		{ 4, 4, 2, 4, 2, 6, 6, 2 },
		{ 6, 2, 4, 2, 4, 2, 6, 4 },
		{ 2, 2, 4, 6, 4, 2, 4, 6 }
	};
	
	static constexpr uint8_t target[8][8] =
	{
		{ 1, 7, 6, 6, 3, 4, 4, 7 }, 
		{ 2, 5, 7, 7, 2, 6, 6, 0 },
		{ 3, 6, 4, 1, 5, 5, 3, 1 }, 
		{ 4, 2, 5, 0, 4, 7, 7, 2 }, 
		{ 5, 0, 0, 3, 7, 2, 5, 3 }, 
		{ 6, 4, 2, 2, 6, 3, 1, 4 },
		{ 7, 1, 1, 5, 0, 0, 2, 5 }, 
		{ 0, 3, 3, 4, 1, 1, 0, 6 }
	};
	
	static constexpr uint8_t adjust[8][8] = 
	{
		{ 0, 0, 0, 1, 3, 2, 1, 1 }, 
		{ 0, 1, 0, 1, 1, 2, 1, 6 }, 
		{ 0, 1, 2, 1, 2, 1, 3, 4 },
		{ 0, 1, 2, 3, 1, 2, 1, 2 },
		{ 0, 1, 2, 1, 3, 4, 3, 4 },
		{ 0, 1, 1, 2, 1, 4, 5, 2 }, 
		{ 0, 1, 2, 1, 3, 2, 5, 4 },
		{ 1, 1, 2, 3, 3, 2, 4, 6 }
	};
	
	uint64_t block;
	uint8_t rest;
	
public:
	number(uint64_t prime): block(prime / 30), rest(key[prime % 30]) {}
	
	number(uint64_t prime, uint64_t begin)
	{
		uint64_t multiplier = std::max(prime, begin / prime + (begin % prime != 0));
		while (value[key[multiplier % 30]] != multiplier % 30)
			++multiplier;
		block = prime * multiplier / 30;
		rest = key[prime * multiplier % 30];
	}
	
	uint64_t get_block() const { return block; }
	uint8_t get_rest() const { return rest; }
	uint64_t get_number() const { return block * 30 + value[rest]; }
	
	number &operator += (const number rhs)
	{
		block += times[rest][rhs.rest] * rhs.block + adjust[rest][rhs.rest];
		rest = target[rest][rhs.rest];
		return *this;
	}
};

#endif
