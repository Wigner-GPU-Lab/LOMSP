// Standard C++ includes
#include <future>      // promise, async
#include <thread>      // sleep_for
#include <iostream>    // cout, cin
#include <chrono>      // seconds


// Inter thread data passing with promise-future pairs:
int main()
{
	std::promise<int> promise;
	auto input_function = [](std::promise<int> p_in)
	                      {
								int x = 0;;
								std::cout << "Enter an integer: \n";
								std::cin >> x;
								std::cout << "Integer was set to: " << x << "\n";
								p_in.set_value(x);
						  };

	auto initializer_function = [](std::future<int> f_in)
	                      {
								std::cout << "Initializing...\n";
								std::this_thread::sleep_for( std::chrono::seconds(5) );
								std::cout << "Initialization finished!\n";
								int x = f_in.get();
								std::cout << "Recieved x = " << x << "\n";
						  };

	// start initializer function:
	auto future_init = std::async( std::launch::async, initializer_function, promise.get_future() );
	// start input function:
	auto future_input = std::async( std::launch::async, input_function, std::move(promise) );

	future_init.wait();
	future_input.wait();
	std::cout << "Sample 5 finished.\n";

	return 0;
}