#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <list>
#include <memory>

class Revision;
class VersionedI;

class Segment : public std::enable_shared_from_this<Segment>
{
public:
	Segment();
	Segment(std::shared_ptr<Segment> parent);
	void Release();
	void Collapse(std::shared_ptr<Revision> main);

	std::shared_ptr<Segment> parent;
	int version;
	int refcount;
	std::list<VersionedI*> written;
	static int versionCount;
};

#endif