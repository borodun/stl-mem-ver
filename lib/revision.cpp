#include "revision.h"
#include "versioned.h"
#include "segment.h"

thread_local std::shared_ptr<Revision> Revision::currentRevision = std::make_shared<Revision>();

Revision::Revision() {
    std::shared_ptr<Segment> s = std::make_shared<Segment>();
    root = s;
    current = s;
}

Revision::Revision(std::shared_ptr<Segment> my_root, std::shared_ptr<Segment> my_current) {
    root = my_root;
    current = my_current;
}

std::shared_ptr<Revision> ForkRevision(std::function<void ()> func) {
	auto s = std::make_shared<Segment>(Revision::currentRevision->current);
	std::shared_ptr<Revision>  r = std::make_shared<Revision>(Revision::currentRevision->current, s);
	Revision::currentRevision->current->Release();
	Revision::currentRevision->current = std::make_shared<Segment>(Revision::currentRevision->current);

	std::shared_ptr<Revision> previous = Revision::currentRevision;
	r->thread = std::thread([func, r, previous]() {
		Revision::currentRevision = r;
		// TODO: try catch
		func();
		Revision::currentRevision = previous;
	});

	return r;
}

void JoinRevision(std::shared_ptr<Revision> join) {
    //TODO: try catch
    join->thread.join();
    std::shared_ptr<Segment> s = join->current;
    while (s != join->root) {
        for (auto v: s->written) {
            v->Merge(Revision::currentRevision, join, s);
        }
        s = s->parent;
    }

    join->current->Release();
    Revision::currentRevision->current->Collapse(Revision::currentRevision);
}
