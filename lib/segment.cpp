#include "segment.h"
#include "versioned.h"
#include "revision.h"

int Segment::versionCount = 0;

Segment::Segment() {
    parent = nullptr;

    version = versionCount++;
    refcount = 1;
}

Segment::Segment(std::shared_ptr<Segment> my_parent) {
    parent = my_parent;
    if (parent != nullptr)
        parent->refcount++;

    version = versionCount++;
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

