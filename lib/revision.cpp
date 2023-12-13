#include "revision.h"
#include "versioned.h"
#include "segment.h"

thread_local Revision *Revision::currentRevision;

Revision::Revision() {
    std::shared_ptr<Segment> s = std::make_shared<Segment>();
    root = s;
    current = s;
    currentRevision = this;
}

Revision::Revision(std::shared_ptr<Segment> my_root, std::shared_ptr<Segment> my_current) {
    root = my_root;
    current = my_current;
}

void Revision::Join(Revision &join) {
    //TODO: try catch
    join.thread.join();
    Segment *s = join.current.get();
    while (s != join.root.get()) {
        for (auto v: s->written) {
            v->Merge(*this, join, *s);
        }
        s = s->parent.get();
    }

    join.current.get()->Release();
    current->Collapse(*this);
}
