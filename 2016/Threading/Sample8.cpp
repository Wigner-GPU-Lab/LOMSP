// Standard C++ includes
#include <mutex>              // mutex, unique_lck
#include <condition_variable> // condition_variable
#include <thread>             // thread, sleep_for
#include <iostream>           // cout, cin


// Inter thread data passing with conditional variables:
int main()
{
	int x;
	std::condition_variable cv;
	std::mutex mutex;
	auto input_function = [&]()
	{
		while(1)
		{
			int i = 0;
			std::cout << "Enter an integer: \n";
			std::cin >> i;
			
			{
				std::unique_lock<std::mutex> lock(mutex);
				x = i;
				cv.notify_all(); // wake up waiting threads
			}
		}
	};

	auto output_function = [&]()
	{
		while(1)
		{
			int i = 0;
			{
				std::unique_lock<std::mutex> lock(mutex);
				cv.wait(lock); // wait for the cv to be notified
				i = x;
			}
			std::cout << "Integer was set to: " << i << "\n";
		}
	};

	
	std::thread(input_function).detach();
	std::thread(output_function).detach();

	std::this_thread::sleep_for(std::chrono::minutes(1));
	std::cout << "Sample 8 finished.\n";

	return 0;
}