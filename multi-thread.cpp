#include <iostream>
#include <cstring>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>

#include "bitset.hpp"

typedef int num;
// struct to pass as argument to threads
struct data { std::vector<num> &result; num begin, limit; };
typedef std::pair<num, num> num_pair;

const int max_threads = std::thread::hardware_concurrency();
const int wheel_size = 2 * 3 * 5 * 7 * 11 * 13;
const int wheel_count = 6; // number of primes in the wheel
const int wheel_primes[wheel_count] = {2, 3, 5, 7, 11, 13};
const int segment_size = wheel_size * 4;

usr::bitset<segment_size> offset_wheel; // wheel to use in the sieve
usr::bitset<wheel_size> normal_wheel; // wheel to speed up trial division

int sieve_limit = 1e9; // default sieve limit

std::vector<int> base_primes;
std::vector<num> *thread_primes;

num to_num(char *s)
{
	num res = 0;
	for (int i = 0; s[i] >= '0' && s[i] <= '9'; ++i)
		res = res * 10 + s[i] - '0';
	return res;
}

bool check_prime(const int number)
{
	// check if number is already marked in the wheel
	if (normal_wheel.test(number % wheel_size))
		return false;
	// factor by trial division
	int limit = sqrt(number);
	for (int prime : base_primes)
		if (number % prime == 0) return false;
		else if (prime > limit) return true;
	return true;
}

void init_wheel(int offset)
{
	for (int prime : wheel_primes)
	{
		for (int current = 0; current < segment_size + offset; current += prime)
			if (current >= offset)
				offset_wheel.set(current - offset);
		for (int current = 0; current < wheel_size; current += prime)
			normal_wheel.set(current);
	}
}

void init_primes(const int limit)
{
	// construct base_primes by trial division
	int current = wheel_primes[wheel_count - 1];
	while ((current += 2) < limit)
		if (check_prime(current))
			base_primes.emplace_back(current);
}

void sieve(const data parsed)
{
	std::vector<num> &result = parsed.result; // result vector
	num begin = parsed.begin; // current starting point of sieve
	const num limit = parsed.limit; // sieve limit

	usr::bitset<segment_size> bitset; // bitset to mark composite numbers
	std::vector<std::pair<num, num>> bucket; // vector to store primes used for sieving
		
	// initialize bucket
	for (num prime : base_primes)
		if ((limit - 1) / prime + 1 <= prime) break;
		else
		{
			num first = std::max(prime * prime, ((begin - 1) / prime + 1) * prime);
			if (first % 2 == 0) first += prime;
			bucket.push_back({prime * 2, first - begin});
		}

	// sieve
	while (begin != limit)
	{			
		// initialize bitset from wheel
		bitset = offset_wheel;
		
		// iterate over all primes in bucket
		for (std::pair<int, int> &cur : bucket)
		{
			while (cur.second < segment_size)
			{
				bitset.set(cur.second);
				cur.second += cur.first;
			}
			cur.second -= segment_size;
		}
	
		//iterate over bitset to find primes
		num number = bitset.extract();
		while (number < segment_size)
		{
			result.push_back(begin + number);
			number = bitset.extract();
		}
				
		begin += segment_size;
	}
}

int main(int argc, char **argv)
{	
	std::cout << "Detected " << max_threads << " usable threads\n";

	// get enviroment arguments
	if (argc >= 2) sieve_limit = to_num(argv[1]);
	std::cout << "Sieve limit set to " << sieve_limit << '\n';
	
	// start clock
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	
	// set lower bound for sieve
	int sieve_begin = sqrt(sieve_limit) + 1;
	int	sieve_worksize = (sieve_limit - sieve_begin) / segment_size;
	sieve_begin = sieve_limit - sieve_worksize * segment_size;
	
	// initialize base prime numbers and prime wheel
	init_wheel(sieve_begin % wheel_size);
	init_primes(sieve_begin);
	
	// spawn threads
	thread_primes = new std::vector<num> [max_threads];
	std::thread *threads = new std::thread [max_threads];
	for (int i = 0; i < max_threads; ++i)
	{
		// calculate worksize for this thread
		const int thread_worksize = (sieve_worksize - 1) / (max_threads - i) + 1;
		const int thread_limit = sieve_begin + thread_worksize * segment_size;
		// spawn thread
		threads[i] = std::thread(sieve, (data){thread_primes[i], sieve_begin, thread_limit});
		// decrement total worksize and increment lower bound
		sieve_worksize -= thread_worksize;
		sieve_begin = thread_limit;
	}
	
	// join threads
	for (int i = 0; i < max_threads; ++i) threads[i].join();
	
	// end clock
	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();
	
	// count the amount of primes calculated
	int prime_count = wheel_count + base_primes.size();
	for (int i = 0; i < max_threads; ++i)
		prime_count += thread_primes[i].size();
		
	std::cout << "Counted " << prime_count << " primes!\n";
	std::cout << "Execution took ";
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count() / 1000;
	std::cout << "ms\n";
	
	// check prime count (for debugging purposes)
	if (sieve_limit == 1e9 && prime_count != 50847534)
		std::cout << "ERROR: Prime count does not match! (should be 50847534)\n";
}
