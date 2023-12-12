#ifndef __REVISION_H__
#define __REVISION_H__

#include <thread>
#include <memory>
#include "segment.h"

// class Segment;

class Revision
{
public:
	Revision();
	Revision(Segment &root, Segment &current);

	template<typename T>
	Revision *Fork(T&& func);

	void Join(Revision &join);

	std::shared_ptr<Segment> root;
	std::shared_ptr<Segment> current;
	std::thread thread;
};

thread_local static Revision *currentRevision;


template<typename T>
Revision *Revision::Fork(T&& func) {
	Segment *s = new Segment(*(current.get()));
	Revision *r = new Revision(*(current.get()), *s);
	current.get()->Release(); // cannot bring refcount to zero
	Segment *sptr = new Segment(*(current.get()));
	current = std::make_shared<Segment>(*sptr);

	thread = std::thread( [func, r]() {
		Revision *previous = currentRevision;
		currentRevision = r;
		// TODO: try catch?
		func();
		currentRevision = previous;
	});

	return r;
}

#endif