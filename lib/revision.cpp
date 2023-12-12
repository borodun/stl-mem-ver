#include "revision.h"
#include "versioned.h"
#include "segment.h"

// thread_local Revision *Revision::currentRevision;

Revision::Revision() {
    Segment *s = new Segment();
    Revision(*s, *s);
}

Revision::Revision(Segment &my_root, Segment &my_current) {
    root = std::make_shared<Segment>(my_root);
    current = std::make_shared<Segment>(my_current);
}

void Revision::Join(Revision &join) {
    //TODO: try catch?

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
