#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <versioned.h>
#include <list>
#include <sstream>
#include "vs_tree.h"

template <class T>
void checkVals(std::string msg, Versioned<T> &x, Versioned<T> &y, T expectedX, T expectedY) {
	T X = x.Get();
	T Y = y.Get();

	std::string xOk = "BAD",
				yOk = "BAD";

	if (X == expectedX) {
		xOk = "OK";
	}
	if (Y == expectedY) {
		yOk = "OK";
	}

	std::cout << msg <<  ": X=" << X << " (expected " << expectedX << ", " << xOk << ")"
					  << ", Y=" << Y << " (expected " << expectedY << ", " << yOk << ")" << std::endl;

}

template <typename Iterator>
std::string join(Iterator begin, Iterator end, char separator = '.')
{
    std::ostringstream o;
    if(begin != end)
    {
        o << *begin++;
        for(;begin != end; ++begin)
            o  << separator << *begin;
    }
    return o.str();
}

template <typename Container>
std::string join(Container const& c, char separator = '.') {
    return join(std::begin(c), std::end(c), separator);
}

template <typename Container>
void checkContainers(std::string msg, Versioned<Container> &x, Versioned<Container> &y, Container expectedX, Container expectedY) {
	Container X = x.Get();
	Container Y = y.Get();

    std::string xstr = "{" + join(X, ',') + "}",
                ystr = "{" + join(Y, ',') + "}";
    std::string expected_xstr = "{" + join(expectedX, ',') + "}",
                expected_ystr = "{" + join(expectedY, ',') + "}";

	std::string xOk = "BAD",
				yOk = "BAD";

	if (X == expectedX) {
		xOk = "OK";
	}
	if (Y == expectedY) {
		yOk = "OK";
	}

	std::cout << msg <<  ": X=" << xstr << " (expected " << expected_xstr << ", " << xOk << ")"
					  << ", Y=" << ystr << " (expected " << expected_ystr << ", " << yOk << ")" << std::endl;

}

template <typename VContainer, typename Container>
bool testCompareContainers(std::string msg, VContainer& vx, Container&& ex)
{
	bool result = (vx.size() == ex.size());
	for (auto ivx = vx.begin(), iex = ex.begin(); iex != ex.end(); ivx++, iex++)
	{
		result = result & (*ivx == *iex);
	}
	std::cout << (result ? "OK" : "BAD") << std::endl;

	return result;
}

template<typename _Key, typename _Comp>
void printVsTree(vs::vs_tree<_Key,_Comp>& t)
{
	std::cout << "tree size: " << t.size() << " height: " << t.height() << std::endl;
	std::cout << "tree contains: ";
	for (auto& i: t)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;
}

#endif
