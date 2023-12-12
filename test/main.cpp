#include <versioned.h>
#include <revision.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  Revision main = Revision();
  Versioned<int> x = Versioned<int>(0);
  std::cout << "Val before fork: " << x.Get() << std::endl;

  auto func = [&x]() {
    x.Set(1);
    std::cout << "Val in fork: " << x.Get() << std::endl;
  };

  Revision *join = main.Fork(func);

  x.Set(2);
  std::cout << "Val in main: " << x.Get() << std::endl;

  main.Join(*join);
  std::cout << "Val after join: " << x.Get() << std::endl;
}