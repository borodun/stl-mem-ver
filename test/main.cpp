#include <iostream>
#include <functional>
#include <list>
#include <set>

#include "versioned.h"
#include "revision.h"
#include "utils.h"
#include "vs_set.h"

void test_vals() {
	std::cout << "Testing basic objects" << std::endl;

	Versioned<int> x = Versioned<int>(0);
	Versioned<int> y = Versioned<int>(100);

	checkVals("Val before fork1", x, y, 0, 100);
	auto thread1 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork1", x, y, 0, 100);
		x.Set(1);
		y.Set(101);
		checkVals("Val after set in fork1", x, y, 1, 101);
	});
	x.Set(11);
	checkVals("Val after fork1", x, y, 11, 100);

	sleep(1);
	std::cout << std::endl;

	checkVals("Val before fork2", x, y, 11, 100);
	auto thread2 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork2", x, y, 11, 100);
		x.Set(2);
		checkVals("Val after set in fork2", x, y, 2, 100);
	});
	y.Set(122);
	checkVals("Val after fork2", x, y, 11, 122);

	sleep(1);
	std::cout << std::endl;

	checkVals("Val before fork3", x, y, 11, 122);
	auto thread3 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork3", x, y, 11, 122);
		y.Set(103);
		checkVals("Val after set in fork3", x, y, 11, 103);
	});
	x.Set(33);
	y.Set(133);
	checkVals("Val after fork3", x, y, 33, 133);

	sleep(1);
	std::cout << std::endl;

	checkVals("Val before join1", x, y, 33, 133);
	JoinRevision(thread1);
	checkVals("Val after join1", x, y, 1, 101);

	checkVals("Val before join2", x, y, 1, 101);
	JoinRevision(thread2);
	checkVals("Val after join2", x, y, 2, 101);

	checkVals("Val before join3", x, y, 2, 101);
	JoinRevision(thread3);
	checkVals("Val after join3", x, y, 2, 103);
}

void test_lists() {
	std::cout << "Testing lists" << std::endl;

	Versioned<std::list<int>> x = Versioned<std::list<int>>({0, 1, 2, 3});
	Versioned<std::list<int>> y = Versioned<std::list<int>>({100, 101, 102, 103});

	checkContainers("Val before fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
	auto thread1 = ForkRevision([&x, &y]() {
		checkContainers("Val before set in fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
		x.Set({0, 1, 2, 4});
		y.Set({100, 101, 102, 104});
		checkContainers("Val after set in fork1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
	});
	x.Set({10, 11, 12});
	checkContainers("Val after fork1", x, y, {10, 11, 12}, {100, 101, 102, 103});

	sleep(1);
	std::cout << std::endl;

	checkContainers("Val before join1", x, y, {10, 11, 12}, {100, 101, 102, 103});
	JoinRevision(thread1);
	checkContainers("Val after join1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
}

void test_sets() {
	std::cout << "Testing sets" << std::endl;

	Versioned<std::set<int>> x = Versioned<std::set<int>>({0, 1, 2, 3});
	Versioned<std::set<int>> y = Versioned<std::set<int>>({100, 101, 102, 103});

	checkContainers("Val before fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
	auto join1 = ForkRevision([&x, &y]() {
		checkContainers("Val before set in fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
		x.Set({0, 1, 2, 4});
		y.Set({100, 101, 102, 104});
		checkContainers("Val after set in fork1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
	});
	x.Set({10, 11, 12});
	checkContainers("Val after fork1", x, y, {10, 11, 12}, {100, 101, 102, 103});

	sleep(1);
	std::cout << std::endl;

	checkContainers("Val before join1", x, y, {10, 11, 12}, {100, 101, 102, 103});
	JoinRevision(join1);
	checkContainers("Val after join1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
}

void test_vs_sets_constructors() {
	std::cout << "Testing vs_sets constructors" << std::endl;

	
	vs::vs_set<int> x;
	vs::vs_set<int, std::greater<int>> xx;
	vs::vs_set<int> y{1,2,3,4};
	vs::vs_set<int, std::greater<int>> z{1,2,3,4};
}

void test_vs_sets() {
	std::cout << "Testing vs_sets" << std::endl;

	vs::vs_set<int> x{0,1,2,3};
	vs::vs_set<int> y{100,101,102,103};


	testCompareContainers("Val before fork1", x, std::set<int>{0, 1, 2, 3});
	testCompareContainers("Val before fork1", y, std::set<int>{100, 101, 102, 103});

	auto join1 = ForkRevision([&x, &y]() {
		testCompareContainers("Val before set", x, std::set<int>{0, 1, 2, 3});
		testCompareContainers("Val before set", y, std::set<int>{100, 101, 102, 103});
		x.insert(4);
		y.insert(104);
		testCompareContainers("Val after set", x, std::set<int>{0, 1, 2, 3, 4});
		testCompareContainers("Val after set", y, std::set<int>{100, 101, 102, 103, 104});
	});
	x.insert(5);
	testCompareContainers("Val after fork1", x, std::set<int>{0, 1, 2, 3, 5});
	testCompareContainers("Val after fork1", y, std::set<int>{100, 101, 102, 103});


	sleep(1);
	std::cout << std::endl;

	testCompareContainers("Val before join1", x, std::set<int>{0, 1, 2, 3, 5});
	testCompareContainers("Val before join1", y, std::set<int>{100, 101, 102, 103});
	JoinRevision(join1);
	testCompareContainers("Val after join1", x, std::set<int>{0, 1, 2, 3, 4});
	testCompareContainers("Val after join1", y, std::set<int>{100, 101, 102, 103, 104});
}

int main(int argc, char **argv)
{
	std::vector<std::function<void()>> tests = 
	{test_vals, test_lists, test_sets, test_vs_sets_constructors, test_vs_sets};


	for(auto& i:tests){
		i();
		sleep(1);
		std::cout << std::endl;
	}
	std::cout << "Test done" << std::endl;
}