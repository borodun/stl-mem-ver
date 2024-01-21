#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include "vs_tree.h"
#include "vs_thread.h"
#include "utils.h"

struct MyKey
{
	public:

	char c;
	int count;

	MyKey(std::pair<const char, int>& p)
	: c(p.first), count(p.second) { }

	friend bool
	operator==(const MyKey& __x, const MyKey& __y)
	{ return __x == __y; }

	friend std::ostream& 
	operator<<(std::ostream& os, const MyKey& k)
	{
		os << k.c + ":" + k.count;
		return os;
	}
};

struct MyComp
{
	bool 
	operator()( const MyKey& lhs, const MyKey& rhs ) const
	{ return lhs.c < rhs.c; }
};


class MyStrategy
{
	public:

	void
	merge(vs::_vs_tree<MyKey, MyComp>& dst, vs::_vs_tree<MyKey, MyComp>& src)
	{
		for (auto& i: src)
		{
			auto found = dst.find(i);
			if (found != dst.end())
				merge_same_element(dst, *found, i);
			else
				dst.push(i);
		}
	}

	void
	merge_same_element(vs::_vs_tree<MyKey, MyComp>& dst, MyKey& dstk, MyKey& srck)
	{ dstk.count += srck.count; }

};

vs::vs_tree<MyKey, MyComp, MyStrategy> t;

std::map<char, int> countFrequencies(std::string& str)
{
	std::map<char, int> res;

	for (char c: str)
	{
		res[c]++;
	}

	return res;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "Provide a filename of text file for demo to run";
		return -1;
	}
	
	std::string filename(argv[1]);

	std::ifstream file_main(filename);

	auto thread = vs::thread([&]()
	{
		std::ifstream file_thr(filename);
		std::string line;

		/* no checks that file has at least 2 lines, because example does */
		std::getline(file_thr, line);
		std::getline(file_thr, line);

		auto res = countFrequencies(line);

		for (auto& i: res)
			t.push(MyKey(i));
		
		std::cout << "tree in thread: \n";
		printVsTree(t);
	});

	std::string line;
	std::getline(file_main, line);

	auto res = countFrequencies(line);

	for (auto& i: res)
		t.push(MyKey(i));

	std::cout << "tree before join: \n";
	printVsTree(t);

	thread.join();

	std::cout << "tree after join: \n";
	printVsTree(t);
	return 0;
}