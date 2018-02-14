// Standard C++ includes
#include <thread>      // thread, sleep_for
#include <chrono>      // milliseconds
#include <algorithm>   // generate
#include <random>      // random_device, uniform_int_distribution
#include <mutex>       // mutex, lock_guard
#include <iostream>    // cout


// Interthread data passing with mutex synchronization:
int main()
{
	std::mutex mutex;
	std::vector<int> data; // shared vector protected by the mutex

	std::random_device random_source; // potentially slow, HW random generator

	auto writer_function = [&]()
	{
		while(1)
		{
			std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
			std::lock_guard<std::mutex> guard(mutex); //lock mutex

			data.resize( std::uniform_int_distribution<int>(1, 10)(random_source) ); //when mutex is locked, resize is safe
			std::uniform_int_distribution<int> dist(0, 500);
			std::generate(data.begin(), data.end(), [&](){ return dist(random_source); });//fill vector with random data
			std::cout << "New vector was generated.\n";
		}
	};


	auto reader_function = [&]()
	{
		while(1)
		{
			{
				std::lock_guard<std::mutex> guard(mutex); // lock mutex
				auto sz = data.size();
				std::cout << "Data size: " << sz << "\n";
				for(auto& e : data){ std::cout << e << " "; }
				std::cout << "\n";
				if( sz != data.size() ) // if you comment out mutex locking, you will probably see this size mismatch (set sleep_for to say 50 or less ms)
				{
					std::cout << "Data was updated while reading!!!\n";
					std::this_thread::sleep_for( std::chrono::seconds(30) );
				}
			}
			std::this_thread::sleep_for( std::chrono::milliseconds(1000) );
		}
	};

	std::thread(writer_function).detach();
	std::thread(reader_function).detach();

	std::this_thread::sleep_for(std::chrono::minutes(1));
	std::cout << "Sample 6 finished.\n";

	return 0;
}