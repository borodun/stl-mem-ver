#ifndef __REVISION_H__
#define __REVISION_H__

#include <thread>
#include <memory>
#include "segment.h"
#include "versioned.h"
#include <iostream>
#include <functional>

class Revision
{
public:
	Revision();
	Revision(std::shared_ptr<Segment> root, std::shared_ptr<Segment> current);

	template<typename T>
	Revision *Fork(T&& func);

	void Join(Revision &join);

	std::shared_ptr<Segment> root;
	std::shared_ptr<Segment> current;
	std::thread thread;
	thread_local static Revision *currentRevision;
};

Revision *ForkRevision(std::function<void ()> func);

void JoinRevision(Revision &join);

#endif