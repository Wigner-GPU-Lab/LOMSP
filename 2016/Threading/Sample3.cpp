// Standard C++ includes
#include <thread>     // sleep_for
#include <future>     // async
#include <chrono>     // seconds
#include <iostream>   // cout


// Return the result of a multiplication performed in a separate thread into a future object using std::async:
int main()
{
	auto mul_function = [](int x)
	{
		std::this_thread::sleep_for(std::chrono::seconds(3)); // wait for 3 seconds in this thread.
		return 2*x;
	};

	auto future = std::async( std::launch::async, mul_function, 21 );

	std::cout << "Result of multiplication: " << future.get() << "\n"; // wait on future to be set

	return 0;
}