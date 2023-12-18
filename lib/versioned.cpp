#include "versioned.h"
#include "revision.h"
#include "segment.h"
#include <string>

template <class T>
Versioned<T>::Versioned(T v) {
    Set(Revision::currentRevision, v);
}

template <class T>
T Versioned<T>::Get() {
    return Get(Revision::currentRevision);
}

template <class T>
T Versioned<T>::Get(std::shared_ptr<Revision> r) {
    std::shared_ptr<Segment> s = r->current;

    while (versions.find(s->version) == versions.end()) {
        if (!s->parent)
            break;

        s = s->parent;
    }

    return versions[s->version];
}

template <class T>
void Versioned<T>::Set(T v) {
    Set(Revision::currentRevision, v);
}

template <class T>
void Versioned<T>::Set(std::shared_ptr<Revision> r, T value) {
    if (versions.find(r->current->version) == versions.end()) {
        r->current->written.push_back(this);
    }

    versions[r->current->version] = value;
}

template <class T>
void Versioned<T>::Release(std::shared_ptr<Segment> release) {
    versions.erase(release->version);
}

template <class T>
void Versioned<T>::Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) {
    if (versions.find(main->current->version) == versions.end()) {
        Set(main, versions[parent->version]);
    }
    versions.erase(parent->version);
}

template <class T>
void Versioned<T>::Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) {
    std::shared_ptr<Segment> s = joinRev->current;
    while (versions.find(s->version) == versions.end()) {
        if (!s->parent)
            break;

        s = s->parent;
    }
    if (s == join) {
        Set(main, versions[join->version]);
    }
}

// Needs to be here because of linking porblems
// To fix them everything should be in header file
template class Versioned<int>;
