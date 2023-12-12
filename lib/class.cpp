#include "class.h"
#include <iostream>

using namespace std;

MyClass::MyClass()
{
  x = 20;
}

void MyClass::DoSomething()
{
  cout << x << endl;
}