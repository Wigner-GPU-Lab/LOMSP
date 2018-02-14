// Standard C++ includes
#include <vector>      // vector
#include <random>      // random_device, normal_distribution
#include <numeric>     // accumulate
#include <algorithm>   // generate
#include <iterator>    // distance, next
#include <future>      // future, async
#include <thread>      // hardware_concurrency
#include <chrono>      // high_resolution_clock, duration_cast, microseconds
#include <iostream>    // cout


// Process a large vector with many threads via async:
int main()
{
	std::vector<double> vec(10'000'000);

	std::random_device random_source; // potentially slow, HW random generator
    std::normal_distribution<double> dist(5.0, 10.0); // normal distribution with average 5 and sigma = 10, generating doubles
	std::generate( vec.begin(), vec.end(), [&](){ return dist(random_source); } ); // fill the vector with random numbers

	auto averager = [](auto it0, auto it1) // lambda to average numbers between two iterators
	{
		using value = typename decltype(it0)::value_type;
		const auto difference = std::distance(it0, it1);
		const auto sum = std::accumulate(it0, it1, static_cast<value>(0), [](const value& x, const value& y){ return x+y; });
		                                                        //, std::plus<value>{}); // STL pre-defined functor
																//);                     // which is also default, may be omitted
		return sum / difference;
	};

	const int max_num_of_threads = (int)std::thread::hardware_concurrency(); // query number of threads
	std::vector<std::future<double>> futures(max_num_of_threads);
	
	const auto time0 = std::chrono::high_resolution_clock::now();

	// start threads:
	auto step = 1 + (int)vec.size() / max_num_of_threads;
	for(int n=0; n<max_num_of_threads; ++n )
	{
		auto it0 = std::next(vec.begin(), std::max( n    * step, 0)              );
		auto it1 = std::next(vec.begin(), std::min((n+1) * step, (int)vec.size()));
		futures[n] = std::async( std::launch::async, averager, it0, it1 );
	}

	// wait on the futures and add results when they become available:
	const auto result = std::accumulate(futures.begin(), futures.end(), 0.0, [](double acc, std::future<double>& f){ return acc + f.get(); } );

	const auto time1 = std::chrono::high_resolution_clock::now();

	std::cout << "Average is: " << result / (double)max_num_of_threads << "\n";
	std::cout << "Elapsed time is: " << std::chrono::duration_cast<std::chrono::microseconds>(time1-time0).count() << " usec\n";

	return 0;
}