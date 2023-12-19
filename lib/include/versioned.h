#ifndef __VERSIONED_H__
#define __VERSIONED_H__

#include <iostream>
#include <memory>
#include <map>

class Segment;
class Revision;

/**
 * @brief Interface for all Versioned classes
 *
 */
class VersionedI
{
public:
	virtual void Release(std::shared_ptr<Segment> release) = 0;
	virtual void Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) = 0;
	virtual void Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) = 0;
};

/**
 * @brief Wrapper to make any class Versioned
 *
 * @tparam T Class that needs to be versioned
 */
template <class T>
class Versioned : public VersionedI {
public:
	/**
	 * @brief Map of all versions of specified object
	 *
	 * @details Segment versions where that object was changed are used as a keys
	 * @see Segment
	 */
	std::map<int, T> versions;

	/**
	 * @brief Construct a new Versioned object from your object
	 *
	 * @param val Your object
	 */
	Versioned(T val);

	/**
	 * @brief Get the current value of the object in the current Revision
	 *
	 * @return T Object value
	 * @see Revision
	 */
	T Get();

	/**
	 * @brief Set new value of the object
	 *
	 * @param v New object value
	 */
	void Set(T v);

	/**
	 * @brief Forget version that was changed in some Segment
	 *
	 * @param release Segment to forget
	 * @see Segment
	 */
	void Release(std::shared_ptr<Segment> release) override;

	/**
	 * @brief Collapse all changes made in Revision into one
	 *
	 * @param main Revision to start collapsing from
	 * @param parent Segment until which to collapse
	 */
	void Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) override;

	/**
	 * @brief Merge changes from two Revisions
	 *
	 * @param main Revision to merge into
	 * @param joinRev Revision to merge
	 * @param join Segment to which to merge
	 */
	void Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) override;

private:

	/**
	 * @brief Get value of versioned object by Revision
	 *
	 * @param r Revision to use
	 * @return T Object value
	 */
	T Get(std::shared_ptr<Revision> r);

	/**
	 * @brief Set object value in specified Revision
	 *
	 * @param r Revision to use
	 * @param value New object value
	 */
	void Set(std::shared_ptr<Revision> r, T value);
};

#endif