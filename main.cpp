#include <cstdio>
#include <vector>
#include <bitset>

#include <algorithm>
#include <thread>
#include <mutex>

#include "primelist.hpp"

const uint32_t SIEVE_LIMIT = 1e9;

const uint32_t THREADS = 8;

const uint32_t WHEEL_SIZE = 2 * 3 * 5 * 7;
const std::bitset<WHEEL_SIZE> WHEEL("011111111101011101011101111101011111011101011101111101111101011111011101011111011101111101111111011101011101011101111111011111011101111101011101111101011111011111011101011101111101011111011101011101011111111101");
const uint32_t WHEEL_HOLES = 48;
const uint32_t HOLES[WHEEL_HOLES + 1] = {1, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 121, 127, 131, 137, 139, 143, 149, 151, 157, 163, 167, 169, 173, 179, 181, 187, 191, 193, 197, 199, 209, 211};

const uint32_t BLOCK_SIZE = 512;
const uint32_t BLOCK_COUNT = (SIEVE_LIMIT - 1) / (BLOCK_SIZE * WHEEL_SIZE) + 1;

uint32_t count;
std::mutex count_mutex;
std::mutex stdio_mutex;

struct data
{
	const uint32_t inc;
	uint32_t num, pos;
	data(const uint32_t inc, uint32_t num): inc(inc)
	{
		if (num > inc * inc)
		{
			num = ((num - 1) / inc + 1) * inc;
			while (WHEEL.test(num % WHEEL_SIZE))
				num += inc;
		}
		else num = inc * inc;
		this->num = num;
		pos = std::lower_bound(HOLES, HOLES + WHEEL_HOLES, num / inc % WHEEL_SIZE) - HOLES;
	}
	void increment()
	{
		num += inc * (HOLES[pos + 1] - HOLES[pos]);
		pos = (pos == WHEEL_HOLES - 1 ? 0 : pos + 1);
	}
};

void sieve_segment(const uint32_t START_BLOCK, const uint32_t END_BLOCK)
{
	uint32_t internal_count = 0;
	std::vector<data> bucket;
	std::bitset<BLOCK_SIZE * WHEEL_SIZE> bitset;
	for (const uint32_t prime : SIEVING_PRIMES)
		bucket.emplace_back(prime, START_BLOCK * BLOCK_SIZE * WHEEL_SIZE);
	for (uint32_t block = START_BLOCK; block < END_BLOCK; ++block)
	{
		const uint32_t start_num = block * BLOCK_SIZE * WHEEL_SIZE;
		const uint32_t last_num = (block + 1) * BLOCK_SIZE * WHEEL_SIZE;
		bitset.reset();
		for (data &dat : bucket)
			while (dat.num < last_num)
			{
				bitset.set(dat.num - start_num);
				dat.increment();
			}
		if (last_num <= SIEVE_LIMIT)
			for (uint32_t offset = 0; offset < BLOCK_SIZE * WHEEL_SIZE; offset += WHEEL_SIZE)
				for (uint32_t i = 0; i < WHEEL_HOLES; ++i)
					internal_count += !bitset.test(offset + HOLES[i]);
		else
			for (uint32_t offset = 0; offset < BLOCK_SIZE * WHEEL_SIZE; offset += WHEEL_SIZE)
				for (uint32_t i = 0; i < WHEEL_HOLES; ++i)
					internal_count += !bitset.test(offset + HOLES[i]) && start_num + offset + HOLES[i] < SIEVE_LIMIT;
	}
	count_mutex.lock();
	count += internal_count;
	count_mutex.unlock();
}

int main()
{
	printf("Sieving to %d using %d threads\n", SIEVE_LIMIT, THREADS);
	std::vector<std::thread*> threads;
	for (uint32_t block = 0, job_size; block < BLOCK_COUNT; block += job_size)
	{
		job_size = (BLOCK_COUNT - block - 1) / (THREADS - threads.size()) + 1;
		stdio_mutex.lock();
		printf("  New thread [#%08d-#%08d]\n", block, block + job_size);
		stdio_mutex.unlock();
		threads.push_back(new std::thread(sieve_segment, block, block + job_size));
	}
	for (std::thread *thread : threads)
		thread->join();
	printf("Counted %d primes\n", count + 3); // 4 wheel primes - number 1 counted as prime
	return 0;
}
