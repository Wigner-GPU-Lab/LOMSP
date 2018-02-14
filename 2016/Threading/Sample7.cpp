// Standard C++ includes
#include <vector>       // vector
#include <chrono>       // seconds, minutes
#include <thread>       // thread, sleep_for
#include <random>       // random_device, uniform_int_distribution
#include <shared_mutex> // shared_mutex, unique_lock
#include <string>       // string, to_string
#include <iostream>     // cout


// Interthread data passing with shared_mutex synchronization:
int main()
{
	std::shared_mutex mutex; // shared mutex: multiple threads can read, only one can write!
	std::vector<int> data; // protected data

	std::random_device random_source;

	auto writer_function = [&]()
	{
		while(1)
		{
			std::this_thread::sleep_for( std::chrono::seconds(5) );
									
			std::unique_lock<std::shared_mutex> guard(mutex); // unique lock mutex for exclusive write access

			data.resize( std::uniform_int_distribution<int>(1, 10)(random_source) ); // resize vector safely
			std::uniform_int_distribution<int> dist(0, 500);
			std::generate(data.begin(), data.end(), [&](){ return dist(random_source); });
			std::cout << "New vector was generated.\n";
		}
	};


	auto reader_function = [&](int index)
	{
		auto name = std::string("thread:") + std::to_string(index); // name of this thread
		while(1)
		{
			{
				std::shared_lock<std::shared_mutex> guard(mutex); // shared lock for shared reading
				if(index < (int)data.size())
				{
					auto str = name + " found " + std::to_string(data[index]) + "\n";
					std::cout << str;
				}
			}
			std::this_thread::sleep_for( std::chrono::seconds(2) );
		}
   };

	std::thread(writer_function).detach();
	for(int i=0; i<10; ++i)
	{
		std::thread(reader_function, i).detach(); // start 10 threads that try to read from the vector if possible
	}

	std::this_thread::sleep_for(std::chrono::minutes(1));
	std::cout << "Sample 7 finished.\n";

	return 0;
}