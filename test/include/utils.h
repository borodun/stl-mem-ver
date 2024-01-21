#ifndef ___UTILS_H__
#define ___UTILS_H__

#include <iostream>
#include <functional>
#include <list>
#include <set>
#include <sstream>

#include "versioned.h"
#include "revision.h"
#include "vs_set.h"
#include "vs_queue.h"
#include "vs_stack.h"
#include "vs_tree.h"
#include "vs_thread.h"

template<typename _Key, typename _Comp, typename _Strategy = vs::vs_tree_strategy<_Key,_Comp>>
void printVsTree(vs::vs_tree<_Key, _Comp, _Strategy>& t)
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