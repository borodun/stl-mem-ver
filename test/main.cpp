#include <versioned.h>
#include <revision.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	Revision main = Revision();
	Versioned<int> x = Versioned<int>(0);

	auto func1 = [&x]() {
		int val = 1;
		std::cout << "Val before set in fork" << val <<  ": " << x.Get() << " (expected 0)" << std::endl;
		x.Set(val);
		std::cout << "Val after set in fork" << val <<  ": " << x.Get() << " (expected 1)" << std::endl;
	};

	std::cout << "Val before fork1: " << x.Get() << " (expected 0)" << std::endl;
	Revision *join1 = main.Fork(func1);
	x.Set(11);
	std::cout << "Val after fork1: " << x.Get() << " (expected 11)" << std::endl;

	auto func2 = [&x]() {
		int val = 2;
		std::cout << "Val before set in fork" << val <<  ": " << x.Get() << " (expected 11)" << std::endl;
		x.Set(val);
		std::cout << "Val after set in fork" << val <<  ": " << x.Get() << " (expected 2)" << std::endl;
	};

	sleep(1);
	printf("\n");

	std::cout << "Val before fork2: " << x.Get() << " (expected 11)" << std::endl;
	Revision *join2 = main.Fork(func2);
	x.Set(22);
	std::cout << "Val after fork2: " << x.Get() << " (expected 22)" << std::endl;

	auto func3 = [&x]() {
		int val = 3;
		std::cout << "Val before set in fork" << val <<  ": " << x.Get() << " (expected 22)" << std::endl;
		x.Set(val);
		std::cout << "Val after set in fork" << val <<  ": " << x.Get() << " (expected 3)" << std::endl;
	};

	sleep(1);
	printf("\n");

	std::cout << "Val before fork3: " << x.Get() << " (expected 22)" << std::endl;
	Revision *join3 = main.Fork(func3);
	x.Set(33);
	std::cout << "Val after fork3: " << x.Get() << " (expected 33)" << std::endl;

	sleep(1);
	printf("\n");

	std::cout << "Val before join1: " << x.Get() << " (expected 33)" << std::endl;
	main.Join(*join1);
	std::cout << "Val after join1: " << x.Get() << " (expected 1)" << std::endl;
	std::cout << "Val before join2: " << x.Get() << " (expected 1)" << std::endl;
	main.Join(*join2);
	std::cout << "Val after join2: " << x.Get() << " (expected 2)" << std::endl;
	std::cout << "Val before join3: " << x.Get() << " (expected 2)" << std::endl;
	main.Join(*join3);
	std::cout << "Val after join3: " << x.Get() << " (expected 3)" << std::endl;
}