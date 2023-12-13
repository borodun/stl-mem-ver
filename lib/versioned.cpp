#include "versioned.h"
#include "revision.h"
#include "segment.h"
#include <string>

template <class T>
Versioned<T>::Versioned(T v) {
    Set(*Revision::currentRevision, v);
}

template <class T>
T Versioned<T>::Get() {
    return Get(*Revision::currentRevision);
}

template <class T>
T Versioned<T>::Get(Revision &r) {
    Segment *s = r.current.get();

    while (versions.find(s->version) == versions.end()) {
        if (s->parent.get() == nullptr)
            break;

        s = s->parent.get();
    }

    return versions[s->version];
}

template <class T>
void Versioned<T>::Set(T v) {
    Set(*Revision::currentRevision, v);
}

template <class T>
void Versioned<T>::Set(Revision &r, T value) {
    if (versions.find(r.current.get()->version) == versions.end()) {
        r.current.get()->written.push_back(this);
    }

    versions[r.current.get()->version] = value;
}

template <class T>
void Versioned<T>::Release(Segment &release) {
    versions.erase(release.version);
}

template <class T>
void Versioned<T>::Collapse(Revision &main, Segment &parent) {
    if (versions.find(main.current.get()->version) == versions.end()) {
        Set(main, versions[parent.version]);
    }
    versions.erase(parent.version);
}

template <class T>
void Versioned<T>::Merge(Revision &main, Revision &joinRev, Segment &join) {
    Segment *s = joinRev.current.get();
    while (versions.find(s->version) == versions.end()) {
        if (s->parent.get() == nullptr)
            break;
        s = s->parent.get();
    }
    if (s == &join) {
        Set(main, versions[join.version]);
    }
}

// Needs to be here because of linking porblems
// To fix them everything should be in header file
template class Versioned<int>;
