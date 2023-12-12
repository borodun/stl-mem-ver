#include "segment.h"
#include "versioned.h"
#include "revision.h"

Segment::Segment() {
    parent = nullptr;
    parent->refcount++;

    version = 1;
    refcount = 1;
}

Segment::Segment(Segment &my_parent) {
    parent = std::make_shared<Segment>(my_parent);
    if (parent != NULL)
        parent->refcount++;

    version = 1;
    refcount = 1;
}

void Segment::Release() {
    if (--refcount == 0) {
        for (auto v: written) {
            v->Release(*this);
        }
        if (parent != NULL)
            parent->Release();
    }
}

void Segment::Collapse(Revision &main) {
    while (parent.get() != main.root.get() && parent->refcount == 1) {
        for(auto v : parent->written) {
            v->Collapse(main, *(parent.get()));
        }
        parent = parent->parent; // remove parent
    }
}

