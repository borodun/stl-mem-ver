#include "revision.h"
#include "versioned.h"
#include "segment.h"
#include <sstream>

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

void PrintRevision(std::shared_ptr<Revision> revision) {
    if (revision == nullptr) {
		std::cout << "Revision is null" << std::endl;
        return;
    }

    std::shared_ptr<Segment> s = revision->current;

    std::ostringstream o;
    o << std::endl;
    while (s) {
        o << "thread: "<< std::this_thread::get_id() << ", segment ver:" << s->version
             << ", addr: " << s << ", refcount: " << s->refcount << ", written size: "  << s->written.size() << std::endl;
        s = s->parent;
    }

    std::cout << o.str() << std::endl;
}
