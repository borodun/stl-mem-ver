#include "class.h"
#include <iostream>

using namespace std;

MyClass::MyClass(int new_x)
{
  x = new_x;
}

void MyClass::DoSomething()
{
  cout << x << endl;
}