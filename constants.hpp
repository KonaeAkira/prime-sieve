#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <cmath>
#include <array>

#include "bitset2/bitset2.hpp"
namespace bs2 = Bitset2;

constexpr uint64_t SIEVE_LIMIT = 1e9;
constexpr uint64_t SIEVE_LIMIT_SQRT = std::sqrt(SIEVE_LIMIT) + 1;

constexpr uint64_t WHEEL_PRIMES[6] = {2, 3, 5, 7, 11, 13};

constexpr uint64_t get_wheel_size()
{
	uint64_t size = 1;
	for (const uint64_t prime : WHEEL_PRIMES)
		size *= prime;
	return size;
}
constexpr uint64_t WHEEL_SIZE = get_wheel_size();

constexpr bs2::bitset2<WHEEL_SIZE> get_wheel()
{
	bs2::bitset2<WHEEL_SIZE> bitset;
	for (const uint64_t prime : WHEEL_PRIMES)
		for (uint64_t i = 0; i < WHEEL_SIZE; i += prime)
			bitset.set(i);
	return bitset;
}
constexpr bs2::bitset2<WHEEL_SIZE> WHEEL(get_wheel());

constexpr uint64_t HOLES_COUNT = WHEEL_SIZE - WHEEL.count();

constexpr std::array<uint64_t, HOLES_COUNT + 1> get_holes()
{
	std::array<uint64_t, HOLES_COUNT + 1> array = {};
	for (uint64_t i = 0, j = 0; i < WHEEL_SIZE; ++i)
		if (!WHEEL.test(i)) array[j++] = i;
	array[HOLES_COUNT] = array[0] + WHEEL_SIZE;
	return array;			
}
constexpr std::array<uint64_t, HOLES_COUNT + 1> HOLES = get_holes();

constexpr uint64_t get_sieve_primes_count()
{
	bs2::bitset2<SIEVE_LIMIT_SQRT> bitset;
	uint64_t count = 0;
	for (uint64_t i = 3; i < SIEVE_LIMIT_SQRT; i += 2)
		if (!bitset.test(i) && !WHEEL.test(i % WHEEL_SIZE))
		{
			++count;
			for (uint64_t j = i * i; j < SIEVE_LIMIT_SQRT; j += i)
				bitset.set(j);
		}
	return count;
}
constexpr uint64_t SIEVE_PRIMES_COUNT = get_sieve_primes_count();

constexpr std::array<uint64_t, SIEVE_PRIMES_COUNT> get_sieve_primes()
{
	bs2::bitset2<SIEVE_LIMIT_SQRT> bitset;
	std::array<uint64_t, SIEVE_PRIMES_COUNT> array = {};
	uint64_t count = 0;
	for (uint64_t i = 3; i < SIEVE_LIMIT_SQRT; i += 2)
		if (!bitset.test(i) && !WHEEL.test(i % WHEEL_SIZE))
		{
			array[count++] = i;
			for (uint64_t j = i * i; j < SIEVE_LIMIT_SQRT; j += i)
				bitset.set(j);
		}
	return array;
}
constexpr std::array<uint64_t, SIEVE_PRIMES_COUNT> SIEVE_PRIMES = get_sieve_primes();

constexpr uint64_t BLOCK_SIZE = 32;
constexpr uint64_t BLOCK_COUNT = (SIEVE_LIMIT - 1) / (BLOCK_SIZE * WHEEL_SIZE) + 1;

const uint64_t THREADS = std::thread::hardware_concurrency();

#endif
