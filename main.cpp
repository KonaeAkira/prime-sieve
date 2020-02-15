#include <cstdio>
#include <vector>
#include <bitset>

#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>

#include "constants.hpp"

uint64_t count = sizeof(WHEEL_PRIMES) / sizeof(WHEEL_PRIMES[0]) - 1;
std::mutex count_mutex;
std::mutex stdio_mutex;

struct data
{
	const uint64_t inc;
	uint64_t num, pos;
	data(const uint64_t inc, uint64_t num): inc(inc)
	{
		if (num > inc * inc)
		{
			num = ((num - 1) / inc + 1) * inc;
			while (WHEEL.test(num % WHEEL_SIZE))
				num += inc;
		}
		else num = inc * inc;
		this->num = num;
		pos = std::lower_bound(HOLES.begin(), HOLES.end(), num / inc % WHEEL_SIZE) - HOLES.begin();
	}
	void increment()
	{
		num += inc * (HOLES[pos + 1] - HOLES[pos]);
		pos = (pos == HOLES_COUNT - 1 ? 0 : pos + 1);
	}
};

void sieve_segment(const uint64_t START_BLOCK, const uint64_t END_BLOCK)
{
	uint64_t internal_count = 0;
	std::vector<data> bucket;
	std::bitset<BLOCK_SIZE * WHEEL_SIZE> bitset;
	for (const uint64_t prime : SIEVE_PRIMES)
		bucket.emplace_back(prime, START_BLOCK * BLOCK_SIZE * WHEEL_SIZE);
	for (uint64_t block = START_BLOCK; block < END_BLOCK; ++block)
	{
		const uint64_t start_num = block * BLOCK_SIZE * WHEEL_SIZE;
		const uint64_t last_num = (block + 1) * BLOCK_SIZE * WHEEL_SIZE;
		bitset.reset();
		for (data &dat : bucket)
			while (dat.num < last_num)
			{
				bitset.set(dat.num - start_num);
				dat.increment();
			}
		if (last_num <= SIEVE_LIMIT)
			for (uint64_t offset = 0; offset < BLOCK_SIZE * WHEEL_SIZE; offset += WHEEL_SIZE)
				for (uint64_t i = 0; i < HOLES_COUNT; ++i)
					internal_count += !bitset.test(offset + HOLES[i]);
		else
			for (uint64_t offset = 0; offset < BLOCK_SIZE * WHEEL_SIZE; offset += WHEEL_SIZE)
				for (uint64_t i = 0; i < HOLES_COUNT; ++i)
					internal_count += !bitset.test(offset + HOLES[i]) && start_num + offset + HOLES[i] < SIEVE_LIMIT;
	}
	count_mutex.lock();
	count += internal_count;
	count_mutex.unlock();
}

int main()
{
	printf("Sieving to %d using %d threads\n", SIEVE_LIMIT, THREADS);
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	std::vector<std::thread*> threads;
	for (uint64_t block = 0, job_size; block < BLOCK_COUNT; block += job_size)
	{
		job_size = (BLOCK_COUNT - block - 1) / (THREADS - threads.size()) + 1;
		stdio_mutex.lock();
		printf("  New thread [#%05d-#%05d]\n", block, block + job_size);
		stdio_mutex.unlock();
		threads.push_back(new std::thread(sieve_segment, block, block + job_size));
	}
	for (std::thread *thread : threads)
		thread->join();
	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();
	printf("Counted %d primes\n", count);
	printf("Elapsed time: %d ms\n", std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count() / 1000);
	return 0;
}
