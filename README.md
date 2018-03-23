# prime-sieve
A somewhat fast C++ implementation of a prime sieve.

## Benchmarks
Submit a pull request to add your benchmarks.
```
i5-4310U: 1078ms (up to 1e9)
i5-6200U: 1249ms (up to 1e9)
```

## Compiling
You will need a compiler supporting C++11 to compile.
#### GNU/Linux
```
g++ -std=c++11 -o prime-sieve -pthread -Ofast multi-thread.cpp
```
#### Windows
```
g++ -std=c++11 -o prime-sieve -Ofast multi-thread.cpp
```

## Usage
#### GNU/Linux
```
./prime-sieve [limit]
```
#### Windows
```
prime-sieve [limit]
```
The default sieving limit is 1e9. Leave **\[limit]** blank to use the default limit or replace it with the desired limit.

## License
This software is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
