# prime-sieve

## Description
A somewhat fast C++ implementation of a prime sieve.

Not meant for production. This is just the result of some teenager with too much time on their hands.

The algorithm uses the segmented sieve of Eratosthenes with bucket sieve and a modulo 210 wheel.

## Benchmarks
Average time to find all primes below `1e9`.

Submit a pull request to add your benchmarks.

CPU | Single-Threaded | Multi-Threaded
:---: | ---: | ---: |
Intel i5-8265U | 1135 ms | 304 ms

## Compiling
Most compilers with C++11 will do.
#### Linux
```
g++ -std=c++11 -o prime-sieve -pthread -Ofast main.cpp
```
#### Windows
Please use Linux.

## License
This program is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
