#ifndef __MYCLASS_H__
#define __MYCLASS_H__

class MyClass
{
public:
  MyClass(int x);

  virtual void DoSomething();

private:
  int x;
};

#endif