#include <cstdio>
#include <cmath>
#include <vector>
#include <bitset>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>

#include "number.hpp"

#define unlikely(expr) __builtin_expect(!!(expr), 0)
#define likely(expr) __builtin_expect(!!(expr), 1)

const uint64_t SIEVE_LIM = 1e9;
const uint64_t SIEVE_LIM_SQRT = std::sqrt(SIEVE_LIM);

const uint32_t THREADS = std::thread::hardware_concurrency();

const uint64_t SEGMENT_SIZE = 1 << 13;

void presieve(std::vector<number> &primes)
{
	std::bitset<SIEVE_LIM_SQRT + 1> bitset;
	const uint64_t offset[8] = {6, 4, 2, 4, 2, 4, 6, 2};
	for (uint64_t i = 7, j = 0; i <= SIEVE_LIM_SQRT; i += offset[++j &= 7])
		if (!bitset.test(i))
		{
			primes.emplace_back(i);
			for (uint64_t k = i * i; k <= SIEVE_LIM_SQRT; k += i)
				bitset.set(k);
		}
}

void sieve(const std::vector<number> &primes, const uint64_t begin_segment, const uint64_t end_segment, uint64_t &global_count, std::mutex &mutex)
{
	uint64_t internal_count = 0;
	std::vector<number> bucket;
	bucket.reserve(primes.size());
	for (const number prime : primes)
		bucket.emplace_back(prime.get_number(), begin_segment * SEGMENT_SIZE * 30);
	std::bitset<SEGMENT_SIZE * 8> bitset;
	for (uint64_t segment = begin_segment; segment < end_segment; ++segment)
	{
		const uint64_t begin_block = segment * SEGMENT_SIZE;
		const uint64_t end_block = (segment + 1) * SEGMENT_SIZE;
		bitset.reset();
		for (uint32_t i = 0; i < bucket.size(); ++i)
			while (bucket[i].get_block() < end_block)
			{
				bitset.set((bucket[i].get_block() - begin_block << 3) + bucket[i].get_rest());
				bucket[i] += primes[i];
			}
		if (end_block * 30 <= SIEVE_LIM)
			internal_count += bitset.size() - bitset.count();
		else
		{
			const uint8_t value[8] = {1, 7, 11, 13, 17, 19, 23, 29};
			for (uint64_t i = 0; (begin_block + (i >> 3)) * 30 + value[i & 7] < SIEVE_LIM; ++i)
				internal_count += !bitset.test(i);
			break;
		}
	}
	mutex.lock();
	global_count += internal_count;
	mutex.unlock();
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
	
	uint64_t prime_count = 2;
	std::mutex prime_count_mutex;
	
	// spawn workers
	std::vector<std::thread*> threads;
	uint64_t segment_count = (SIEVE_LIM - 1) / (SEGMENT_SIZE * 30) + 1;
	while (segment_count > 0)
	{
		uint64_t start_segment = segment_count - (segment_count / (THREADS - threads.size()));
		printf("  New thread %04lld-%04lld\n", start_segment, segment_count);
		threads.push_back(new std::thread(sieve, std::ref(primes), start_segment, segment_count, std::ref(prime_count), std::ref(prime_count_mutex)));
		segment_count = start_segment;
	}
	for (auto thread : threads)
		thread->join();
	
	// stop timer
	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();
	
	printf("Counted %lld primes!\n", prime_count);
	printf("Elapsed time: %d ms\n", std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count() / 1000);
	return 0;
}
