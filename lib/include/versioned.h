#ifndef __VERSIONED_H__
#define __VERSIONED_H__

#include <iostream>
#include <map>

class Segment;
class Revision;

class VersionedI
{
public:
	virtual void Release(Segment &release) = 0;
	virtual void Collapse(Revision &main, Segment &parent) = 0;
	virtual void Merge(Revision &main, Revision &joinRev, Segment &join) = 0;
};

template <class T>
class Versioned : public VersionedI {
public:
	std::map<int, T> versions;

	Versioned(T val);

	T Get();
	T Get(Revision &r);

	void Set(T v);
	void Set(Revision &r, T value);

	void Release(Segment &release) override;
	void Collapse(Revision &main, Segment &parent) override;
	void Merge(Revision &main, Revision &joinRev, Segment &join) override;
};

#endif