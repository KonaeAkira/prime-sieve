#include <cstdio>
#include <cmath>
#include <vector>
#include <bitset>
#include <algorithm>
#include <thread>
#include <atomic>
#include <chrono>

#include "number.hpp"

#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define likely(expr) __builtin_expect(!!(expr), 1)

const uint64_t SIEVE_LIM = 1e9;
const uint64_t SIEVE_LIM_SQRT = std::sqrt(SIEVE_LIM);

const uint32_t THREADS = std::thread::hardware_concurrency();

const uint64_t SEGMENT_SIZE = 1 << 13;

std::atomic<uint64_t> global_count = 3;

void presieve(std::vector<number> &primes)
{
	std::bitset<SIEVE_LIM_SQRT + 1> bitset;
	const uint64_t offset[48] = {10, 2, 4, 2, 4, 6, 2, 6, 4, 2, 4, 6, 6, 2, 6, 4, 2, 6, 4, 6, 8, 4, 2, 4, 2, 4, 8, 6, 4, 6, 2, 4, 6, 2, 6, 6, 4, 2, 4, 6, 2, 6, 4, 2, 4, 2, 10, 2};
	for (uint64_t i = 11, j = 0; i <= SIEVE_LIM_SQRT; i += offset[++j %= 48])
		if (!bitset.test(i))
		{
			primes.emplace_back(i);
			for (uint64_t k = i * i; k <= SIEVE_LIM_SQRT; k += i)
				bitset.set(k);
		}
}

void sieve(const std::vector<number> &primes, const uint64_t begin_segment, const uint64_t end_segment)
{
	uint64_t internal_count = 0;
	std::vector<number> bucket;
	bucket.reserve(primes.size());
	for (const number prime : primes)
		bucket.emplace_back(prime.get_number(), begin_segment * SEGMENT_SIZE * 210);
	std::bitset<SEGMENT_SIZE * 48> bitset;
	uint64_t begin_block, end_block;
	for (uint64_t segment = begin_segment; segment < end_segment; ++segment)
	{
		begin_block = segment * SEGMENT_SIZE;
		end_block = (segment + 1) * SEGMENT_SIZE;
		bitset.reset();
		for (uint32_t i = 0; i < primes.size(); ++i)
			while (bucket[i].get_block() < end_block)
			{
				bitset.set((bucket[i].get_block() - begin_block) * 48 + bucket[i].get_rest());
				bucket[i] += primes[i];
			}
		if (end_block * 210 <= SIEVE_LIM)
			internal_count += bitset.size() - bitset.count();
		else
		{
			const uint8_t value[48] = {1, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 121, 127, 131, 137, 139, 143, 149, 151, 157, 163, 167, 169, 173, 179, 181, 187, 191, 193, 197, 199, 209};
			for (uint64_t i = 0; (begin_block + i / 48) * 210 + value[i % 48] < SIEVE_LIM; ++i)
				internal_count += !bitset.test(i);
			break;
		}
	}
	global_count += internal_count;
}

int main()
{
	printf("Sieving to %lld\n", SIEVE_LIM);
	printf("Using %d threads\n", THREADS);

	// start timer
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	
	// generate sieving primes
	std::vector<number> primes;
	presieve(primes);
	
	// spawn workers
	std::vector<std::thread*> threads;
	uint64_t segment_count = (SIEVE_LIM - 1) / (SEGMENT_SIZE * 210) + 1;
	while (segment_count > 0)
	{
		uint64_t start_segment = segment_count - (segment_count / (THREADS - threads.size()));
		printf("  New thread %04lld-%04lld\n", start_segment, segment_count);
		threads.push_back(new std::thread(sieve, std::ref(primes), start_segment, segment_count));
		segment_count = start_segment;
	}
	for (auto thread : threads)
		thread->join();
	
	// stop timer
	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();
	
	printf("Counted %lld primes!\n", global_count.load());
	printf("Elapsed time: %d ms\n", std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count() / 1000);
	return 0;
}
