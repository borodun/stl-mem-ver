#include "versioned.h"
#include "revision.h"
#include "segment.h"

template <class T>
Versioned<T>::Versioned(T v) {
    Set(*currentRevision, v);
}

template <class T>
T Versioned<T>::Get() {
    return Get(*currentRevision);
}

template <class T>
T Versioned<T>::Get(Revision &r) {
    Segment *s = r.current.get();

    while (versions[s->version] == -1) {
        s = s->parent.get();
    }

    return versions[s->version];
}

template <class T>
void Versioned<T>::Set(T v) {
    Set(*currentRevision, v);
}

template <class T>
void Versioned<T>::Set(Revision &r, T value) {
    if (versions[r.current.get()->version] == -1) {
        r.current.get()->written.push_back(this);
    }

    versions[r.current.get()->version] = value;
}

template <class T>
void Versioned<T>::Release(Segment &release) {
    versions[release.version] = -1;
}

template <class T>
void Versioned<T>::Collapse(Revision &main, Segment &parent) {
    if (versions[main.current.get()->version] == -1) {
        Set(main, versions[parent.version]);
    }
    versions[parent.version] = -1;
}

template <class T>
void Versioned<T>::Merge(Revision &main, Revision &joinRev, Segment &join) {
    Segment *s = joinRev.current.get();
    while (versions[s->version] == -1) {
        s = s->parent.get();
    }
    if (s == &join) { // only merge if this was the last write
        Set(main, versions[join.version]);
    }
}

template class Versioned<int>;
