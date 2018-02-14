// Standard C++ includes
#include <thread>     // thread 
#include <future>     // promise
#include <iostream>   // cout


// Return the result of a multiplication performed in a separate thread through a promise object to the linket future:
int main()
{
	std::promise<int> promise;
	std::future<int> future = promise.get_future(); // link future to promise
	auto mul_function = [](std::promise<int> p_in, int x)
	{
		p_in.set_value(2*x);
		std::this_thread::sleep_for(std::chrono::seconds(3)); // wait for 3 seconds in this thread.
	};

	std::thread t(mul_function, std::move(promise), 21 ); // create thread

	std::cout << "Result of multiplication: " << future.get() << "\n"; // wait on future to be set

	t.join(); // wait thread to finish
	std::cout << "Thread finished.\n";

	return 0;
}
