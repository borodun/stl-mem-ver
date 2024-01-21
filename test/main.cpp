#include <iostream>
#include <functional>
#include <list>
#include <set>

#include "versioned.h"
#include "revision.h"
#include "utils.h"
#include "vs_set.h"
#include "vs_queue.h"
#include "vs_stack.h"
#include "vs_tree.h"
#include "vs_thread.h"

void test_vals() {
	std::cout << "Testing basic objects for " << std::this_thread::get_id() << std::endl;

	Versioned<int> x = Versioned<int>(0);
	Versioned<int> y = Versioned<int>(100);

	checkVals("Val before fork1", x, y, 0, 100);
	auto thread1 = vs::thread([&x, &y]() {
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
	auto thread2 = vs::thread([&x, &y]() {
		checkVals("Val before set in fork2", x, y, 11, 100);
		x.Set(2);
		checkVals("Val after set in fork2", x, y, 2, 100);
	});
	y.Set(122);
	checkVals("Val after fork2", x, y, 11, 122);

	sleep(1);
	std::cout << std::endl;

	checkVals("Val before fork3", x, y, 11, 122);
	auto thread3 = vs::thread([&x, &y]() {
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
	thread1.join();
	checkVals("Val after join1", x, y, 1, 101);

	checkVals("Val before join2", x, y, 1, 101);
	thread2.join();
	checkVals("Val after join2", x, y, 2, 101);

	checkVals("Val before join3", x, y, 2, 101);
	thread3.join();
	checkVals("Val after join3", x, y, 2, 103);
}

void test_lists() {
	std::cout << "Testing lists" << std::endl;

	Versioned<std::list<int>> x = Versioned<std::list<int>>({0, 1, 2, 3});
	Versioned<std::list<int>> y = Versioned<std::list<int>>({100, 101, 102, 103});

	checkContainers("Val before fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
	auto thread1 = vs::thread([&x, &y]() {
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
	thread1.join();
	checkContainers("Val after join1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
}

void test_sets() {
	std::cout << "Testing sets" << std::endl;

	Versioned<std::set<int>> x = Versioned<std::set<int>>({0, 1, 2, 3});
	Versioned<std::set<int>> y = Versioned<std::set<int>>({100, 101, 102, 103});

	checkContainers("Val before fork1", x, y, {0, 1, 2, 3}, {100, 101, 102, 103});
	auto thread1 = vs::thread([&x, &y]() {
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
	thread1.join();
	checkContainers("Val after join1", x, y, {0, 1, 2, 4}, {100, 101, 102, 104});
}

void test_vs_sets_constructors() {
	std::cout << "Testing vs_sets constructors" << std::endl;


	vs::vs_set<int> x;
	vs::vs_set<int> y{1,2,3,4};
	vs::vs_set<int, std::greater<int>> xx;
	vs::vs_set<int, std::greater<int>> z{1,2,3,4};
}

void test_vs_sets() {
	std::cout << "Testing vs_sets" << std::endl;

	vs::vs_set<int> x{0,1,2,3};
	vs::vs_set<int> y{100,101,102,103};


	testCompareContainers("Val before fork1", x, std::set<int>{0, 1, 2, 3});
	testCompareContainers("Val before fork1", y, std::set<int>{100, 101, 102, 103});

	auto thread1 = vs::thread([&x, &y]() {
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
	thread1.join();
	testCompareContainers("Val after join1", x, std::set<int>{0, 1, 2, 3, 4, 5});
	testCompareContainers("Val after join1", y, std::set<int>{100, 101, 102, 103, 104});
}

void
test_vs_queue_constructors()
{
	vs::vs_queue<int> q{1,2,3,4};
	q.push(5);
	std::cout << "queue before" << q.front() << " " << q.back() << std::endl;
	q.pop();
	std::cout << "queue after" << q.front() << " " << q.back() << std::endl;
}

void
test_vs_stack_constructors()
{
	vs::vs_stack<int> s{1,2,3,4};
	s.push(5);
	std::cout << "stack before " << s.top() << std::endl;
	s.pop();
	std::cout << "stack after " << s.top() << std::endl;
}

void
test_vs_tree()
{
	vs::vs_tree<int> t{1,2,3,4};

	vs::vs_tree<float, std::greater<float>> tt{1,2,3,4};

	printVsTree(t);
	t.push(6);
	t.push(7);
	printVsTree(t);
	t.push(8);
	t.push(9);
	printVsTree(t);
}


int main(int argc, char **argv)
{
	std::vector<std::function<void()>> tests =
	{test_vals, test_lists, test_sets,
	test_vs_sets_constructors, test_vs_sets,
	test_vs_queue_constructors, test_vs_stack_constructors,
	test_vs_tree};


	for(auto& i:tests){
		i();
		sleep(1);
		std::cout << std::endl;
	}
	std::cout << "Test done" << std::endl;
}