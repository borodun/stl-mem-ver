#include <dlfcn.h>
#include <iostream>
#include "class.h"

using namespace std;

int main(int argc, char **argv)
{
  MyClass* classVar = new MyClass();
  classVar->DoSomething();
}