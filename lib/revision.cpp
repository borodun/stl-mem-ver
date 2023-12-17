#include "revision.h"
#include "versioned.h"
#include "segment.h"

thread_local Revision *Revision::currentRevision = new Revision();

Revision::Revision() {
    std::shared_ptr<Segment> s = std::make_shared<Segment>();
    root = s;
    current = s;
}

Revision::Revision(std::shared_ptr<Segment> my_root, std::shared_ptr<Segment> my_current) {
    root = my_root;
    current = my_current;
}

Revision *ForkRevision(std::function<void ()> func) {
	auto s = std::make_shared<Segment>(Revision::currentRevision->current);
	Revision *r = new Revision(Revision::currentRevision->current, s);
	Revision::currentRevision->current.get()->Release();
	Revision::currentRevision->current = std::make_shared<Segment>(Revision::currentRevision->current);

	Revision *previous = Revision::currentRevision;
	r->thread = std::thread([func, r, previous]() {
		Revision::currentRevision = r;
		// TODO: try catch
		func();
		Revision::currentRevision = previous;
	});

	return r;
}

void JoinRevision(Revision &join) {
    //TODO: try catch
    join.thread.join();
    Segment *s = join.current.get();
    while (s != join.root.get()) {
        for (auto v: s->written) {
            v->Merge(*Revision::currentRevision, join, *s);
        }
        s = s->parent.get();
    }

    join.current.get()->Release();
    Revision::currentRevision->current->Collapse(*Revision::currentRevision);
}
