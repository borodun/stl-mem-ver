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

	std::shared_ptr<Segment> root;
	std::shared_ptr<Segment> current;
	std::thread thread;
	thread_local static std::shared_ptr<Revision> currentRevision;
};

std::shared_ptr<Revision> ForkRevision(std::function<void ()> func);

void JoinRevision(std::shared_ptr<Revision> join);

#endif