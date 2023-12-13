#ifndef __REVISION_H__
#define __REVISION_H__

#include <thread>
#include <memory>
#include "segment.h"
#include <iostream>

// class Segment;

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

// It needs to be in header because of linking problems
template<typename T>
Revision *Revision::Fork(T&& func) {
	auto s = std::make_shared<Segment>(current);
	Revision *r = new Revision(current, s);
	current.get()->Release();
	current = std::make_shared<Segment>(current);

	Revision *previous = currentRevision;
	r->thread = std::thread([func, r, previous]() {
		currentRevision = r;
		// TODO: try catch
		func();
		currentRevision = previous;
	});

	return r;
}

#endif