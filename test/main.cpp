#include <versioned.h>
#include <revision.h>
#include <iostream>


template <class T>
void checkVals(std::string msg, Versioned<T> x, Versioned<T> y, T expectedX, T expectedY) {
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

int main(int argc, char **argv)
{
	Versioned<int> x = Versioned<int>(0);
	Versioned<int> y = Versioned<int>(100);

	checkVals("Val before fork1", x, y, 0, 100);
	Revision *join1 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork1", x, y, 0, 100);
		x.Set(1);
		y.Set(101);
		checkVals("Val after set in fork1", x, y, 1, 101);
	});
	x.Set(11);
	checkVals("Val after fork1", x, y, 11, 100);

	sleep(1);
	printf("\n");

	checkVals("Val before fork2", x, y, 11, 100);
	Revision *join2 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork2", x, y, 11, 100);
		x.Set(2);
		checkVals("Val after set in fork2", x, y, 2, 100);
	});
	y.Set(122);
	checkVals("Val after fork2", x, y, 11, 122);

	sleep(1);
	printf("\n");

	checkVals("Val before fork3", x, y, 11, 122);
	Revision *join3 = ForkRevision([&x, &y]() {
		checkVals("Val before set in fork3", x, y, 11, 122);
		y.Set(103);
		checkVals("Val after set in fork3", x, y, 11, 103);
	});
	x.Set(33);
	y.Set(133);
	checkVals("Val after fork3", x, y, 33, 133);

	sleep(1);
	printf("\n");

	checkVals("Val before join1", x, y, 33, 133);
	JoinRevision(*join1);
	checkVals("Val after join1", x, y, 1, 101);

	checkVals("Val before join1", x, y, 1, 101);
	JoinRevision(*join2);
	checkVals("Val after join1", x, y, 2, 101);

	checkVals("Val before join1", x, y, 2, 101);
	JoinRevision(*join3);
	checkVals("Val after join1", x, y, 2, 103);
}