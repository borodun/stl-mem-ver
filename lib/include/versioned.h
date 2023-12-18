#ifndef __VERSIONED_H__
#define __VERSIONED_H__

#include <iostream>
#include <memory>
#include <map>

class Segment;
class Revision;

class VersionedI
{
public:
	virtual void Release(std::shared_ptr<Segment> release) = 0;
	virtual void Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) = 0;
	virtual void Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) = 0;
};

template <class T>
class Versioned : public VersionedI {
public:
	std::map<int, T> versions;

	Versioned(T val);

	T Get();
	T Get(std::shared_ptr<Revision> r);

	void Set(T v);
	void Set(std::shared_ptr<Revision> r, T value);

	void Release(std::shared_ptr<Segment> release) override;
	void Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) override;
	void Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) override;
};

#endif