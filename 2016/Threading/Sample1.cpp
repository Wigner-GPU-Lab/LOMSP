// Standard C++ includes
#include <thread>     // thread 
#include <iostream>   // cout


// Print the result of a multiplication in a separate thread:
int main()
{
	auto writer_function = [](int x)
	{
		std::cout << "2 * 21 = " << 2*x << "\n";
	};
	
	std::thread t(writer_function, 21 ); // create thread
	
	t.join(); // wait thread to finish
	
	return 0;
}
