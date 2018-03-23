#include <iostream>
#include <cstring>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>

#include "bitset.hpp"

typedef int num;
struct data { std::vector<num> &result, &primes; num begin, limit; };
typedef std::pair<num, num> num_pair;

const int max_threads = std::thread::hardware_concurrency();
const num max_sieve_size = 1 << 15;
num estimated_count, sieve_lim = 1e9;
    
int wheel_count = 4;
int wheel_size;
usr::bitset<max_sieve_size> *wheel;

std::vector<num> *primes;

num to_num(char *s)
{
	num res = 0;
	for (int i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
		res = res * 10 + s[i] - '0';
	return res;
}

inline bool check_prime(const register num number)
{
	if (wheel_size != 0 && wheel[0].test(number % wheel_size))
		return false;
	num limit = sqrt(number);
	for (num &prime : primes[max_threads])
		if (number % prime == 0) return false;
		else if (prime > limit) return true;
	return true;
}

void init_memory()
{
	estimated_count = (float)sieve_lim / log(sieve_lim) * 1.2;
	primes = new std::vector<num> [max_threads + 1];
	for (int i = 0; i < max_threads; ++i)
		primes[i].reserve(estimated_count / max_threads);
}

void init_wheel()
{
	wheel_size = 1;
	for (num &prime : primes[max_threads])
		wheel_size *= prime;
	
	wheel = new usr::bitset<max_sieve_size> [wheel_size];
	
	for (num &prime : primes[max_threads])
		for (int j = 0; j < max_sieve_size + wheel_size; j += prime)
			for (int k = 0; k < wheel_size; ++k)
				if (j >= k && j - k < max_sieve_size)
					wheel[k].set(j - k);
}

void init_primes()
{
	primes[max_threads].push_back(2);
	num limit = sqrt(sieve_lim) + 1;
	for (register num i = 3; i < limit; i += 2)
		if (check_prime(i))
		{
			primes[max_threads].emplace_back(i);
			if (wheel_size == 0 && (int)primes[max_threads].size() == wheel_count)
			{
				init_wheel();
				primes[max_threads].clear();
			}
		}
}

void sieve(const data parsed)
{
	const std::vector<num> &primes = parsed.primes; // primes used for the sieve
	std::vector<num> &result = parsed.result; // result vector
	num begin = parsed.begin; // current starting point of sieve
	const num limit = parsed.limit; // sieve limit
	                       
	const int sieve_size = std::min(max_sieve_size, limit - begin); // size per sieve
	const int sieve_count = (limit - begin - 1) / sieve_size + 1; // amount of times to run the sieve
	usr::bitset<max_sieve_size> bitset; // bitset to mark composite numbers
	
	std::vector<std::pair<num, num>> bucket;
		
	// initialize bucket
	for (register num prime : primes)
		if ((limit - 1) / prime + 1 <= prime) break;
		else
		{
			num first = std::max(prime * prime, ((begin - 1) / prime + 1) * prime);
			if (first % 2 == 0) first += prime;
			bucket.push_back({prime * 2, first - begin});
		}
	
	// sieve
	for (int iter = 0; iter < sieve_count; ++iter)
	{			
		const num target = std::min(limit, begin + sieve_size);
		const num sieve_size = target - begin;
		
		// initialize bitset from wheel
		bitset = wheel[begin % wheel_size];
		
		// iterate over all primes in bucket
		for (std::pair<int, int> &cur : bucket)
		{
			while (cur.second < sieve_size)
			{
				bitset.set(cur.second);
				cur.second += cur.first;
			}
			cur.second -= sieve_size;
		}
	
		//iterate over bitset to find primes
		num number = bitset.extract();
		while (number < sieve_size)
		{
			result.push_back(begin + number);
			number = bitset.extract();
		}
				
		begin += sieve_size;
	}
}

int main(int argc, char **argv)
{	
	std::cout << "Detected " << max_threads << " usable threads\n";

	// get enviroment arguments
	if (argc >= 2) sieve_lim = to_num(argv[1]);
	std::cout << "Sieve limit set to " << sieve_lim << '\n';
	
	// start clock
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	
	init_memory();
	init_primes();
	
	// spawn threads
	std::thread *threads = new std::thread [max_threads];
	const num begin = sqrt(sieve_lim) + 1;
	const num worksize = (sieve_lim - begin - 1) / max_threads + 1;
	for (int i = max_threads - 1; i >= 0; --i)
	{
		const num start = begin + worksize * i;
		const num limit = std::min(start + worksize, sieve_lim);
		threads[i] = std::thread(sieve, (data){primes[i], primes[max_threads], start, limit});
	}
	
	// join threads
	for (int i = 0; i < max_threads; ++i)
		threads[i].join();
	
	// end clock
	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();
	
	num prime_count = wheel_count; // to account for '2', which is not stored
	for (int i = 0; i <= max_threads; ++i)
		prime_count += primes[i].size();
	std::cout << "Counted " << prime_count << " primes!\n";
	std::cout << "Execution took ";
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count() / 1000;
	std::cout << "ms\n";
	
	if (sieve_lim == 1e9 && prime_count != 50847534)
		std::cout << "ERROR: Prime count does not match! (should be 50847534)\n";
}
