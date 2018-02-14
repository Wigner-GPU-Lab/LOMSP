// Standard C++ includes
#include <iostream>    // cout
#include <vector>      // vector
#include <random>      // random_device, normal_distribution
#include <iterator>    // next
#include <algorithm>   // generate, for_each
#include <numeric>     // min, max
#include <future>      // async, future
#include <chrono>      // high_resolution_clock


template <typename T>
struct atomic_wrapper
{
  std::atomic<T> data;

  atomic_wrapper():data(){}
  atomic_wrapper(std::atomic<T> const& a   ):data(a.load()){}
  atomic_wrapper(atomic_wrapper const& copy):data(copy.data.load()){}
  atomic_wrapper& operator=(atomic_wrapper const& copy){ data.store(copy.data.load()); return *this; }
};

// Histogramming with atomics:
int main()
{
	std::vector<double> vec(10'000'000);

	std::random_device random_source;
    std::normal_distribution<double> dist(5.0, 0.5);
	std::generate( vec.begin(), vec.end(), [&](){ return dist(random_source); } );

	std::vector<atomic_wrapper<int>> histogram(30);
	//std::vector<int> histogram(30); // use this to try without atomics
	double low  = 0.0;
	double high = 10.0;
	auto averager = [&](auto it0, auto it1)
	{
		for( auto it=it0; it!=it1; ++it)
		{
			auto index = size_t((histogram.size()-1) * (((*it)-low)/(high-low)));
			if( index > 0 && index < histogram.size()-1 )
			{
				histogram[index].data += 1; // this is the atomic increment
				//histogram[index] += 1; // use this to try without atomics
			}
		}
	};

	int max_num_of_threads = 4; //(int)std::thread::hardware_concurrency();
	std::vector<std::future<void>> futures(max_num_of_threads);

	auto time0 = std::chrono::high_resolution_clock::now();

	// start threads:
	int step = 1 + vec.size() / max_num_of_threads;
	for(int n=0; n<max_num_of_threads; ++n )
	{
		auto it0 = std::next(vec.begin(), std::max( n    * step, 0)              );
		auto it1 = std::next(vec.begin(), std::min((n+1) * step, (int)vec.size()));
		futures[n] = std::async( std::launch::async, averager, it0, it1 );
	}

	// wait on the futures and add results when they become available:
	std::for_each(futures.begin(), futures.end(), [](std::future<void>& f){ f.get(); } );

	int sum = 0;
	for(auto& h : histogram)
	{
		auto q = h.data.load();
		//auto q = h; // use this to try without atomics
		std::cout << q << "\n";
		sum += q;
	}

	auto time1 = std::chrono::high_resolution_clock::now();

	std::cout << "Sum is: " << sum << "\n";
	std::cout << "Elapsed time is: " << std::chrono::duration_cast<std::chrono::milliseconds>(time1-time0).count() << " msec\n";

	// Sample results:
	// wo atomic:  467 ms sum =  7 153 357
	// w  atomic: 1070 ms sum = 10 000 000

	return 0;
}
