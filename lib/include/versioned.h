/**
 * @file  versioned.h
 *
 * @brief Implementation of the Versioned classes and interface.
 */

#ifndef __VERSIONED_H__
#define __VERSIONED_H__

#include <iostream>
#include <memory>
#include <map>
#include "revision.h"
#include "segment.h"

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

/* XXX: not really fits isMergeStrategy concept (no key and container here), but anyway */
template<typename T>
class DefaultMergeStrategy
{
	public:

	void
	merge(T& dst, T& src)
	{
		dst = src;
	}

	// void
	// merge_same_element(T& dst, _Key& dstk, _Key& srck)
	// {
	// 	/* pretend we didnt saw new elements */
	// }
};

/**
 * @brief Wrapper to make any class Versioned
 *
 * @tparam T Class that needs to be versioned
 */
template <class T, typename _Strategy = DefaultMergeStrategy<T>>
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
	Versioned(const T& val);

	/**
	 * @brief Destroy the Versioned object
	 *
	 * Removes all mentions of that versioned object in current Revision
	 */
	~Versioned();

	/**
	 * @brief Get the current value of the object in the current Revision
	 *
	 * @return T Object value
	 * @see Revision
	 */
	const T& Get() const;

	/**
	 * @brief Set new value of the object
	 *
	 * @param v New object value
	 * @param updater Function to update value
	 * 
	 * @return bool result of updater if defined, else true
	 * 
	 * If updater function is defined, object will be updated in-place
	 * instead of overwrite
	 */
	bool Set(const T& v, const std::function<bool(T&)>& updater = nullptr);

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
	const T& Get(std::shared_ptr<Revision> r) const;

	/**
	 * @brief Set object value in specified Revision
	 *
	 * @param r Revision to use
	 * @param value New object value
	 * @param updater Function to update value
	 * 
	 * @return bool result of updater if defined, else true
	 * 
	 * If there is no version for this revision, create it
	 * else overwrite current item
	 */
	bool Set(std::shared_ptr<Revision> r, const T& value, const std::function<bool(T&)>& updater = nullptr);


	/**
	 * @brief Like Set, but use _Strategy to write instead
	 */
	bool SetMerge(std::shared_ptr<Revision> r, T& value);

	/**
	 * @brief Injected from versioned collections
	 */
	_Strategy merge_strategy;
};


// All methods need to be decalred in header due to template linking issues

template <class T, typename _Strategy>
Versioned<T,_Strategy>::Versioned(const T& v) {
    Set(Revision::currentRevision, v);
}

template <class T, typename _Strategy>
inline Versioned<T,_Strategy>::~Versioned()
{
	std::shared_ptr<Segment> s = Revision::currentRevision->current;

	while (s) {
        if (versions.find(s->version) != versions.end()) {
			for (auto it = s->written.begin(); it != s->written.end();){
				if (*it == this)
					it = s->written.erase(it);
				else
					it++;
			}
		}
        s = s->parent;
    }
}

template <class T, typename _Strategy>
const T& Versioned<T,_Strategy>::Get() const{
    return Get(Revision::currentRevision);
}

template <class T, typename _Strategy>
const T& Versioned<T,_Strategy>::Get(std::shared_ptr<Revision> r) const{
    std::shared_ptr<Segment> s = r->current;

    while (versions.find(s->version) == versions.end()) {
        if (!s->parent)
            break;

        s = s->parent;
    }

    return versions.at(s->version);
}

template <class T, typename _Strategy>
bool Versioned<T,_Strategy>::Set(const T& v, const std::function<bool(T&)>& updater) {
	return Set(Revision::currentRevision, v, updater);
}

template <class T, typename _Strategy>
bool Versioned<T,_Strategy>::Set(std::shared_ptr<Revision> r, const T& value, const std::function<bool(T&)>& updater) {
	if (versions.find(r->current->version) == versions.end()) {
		r->current->written.push_back(this);
		versions[r->current->version] = value;
		if (updater){
			return updater(versions[r->current->version]);
		}
	} else {
		if (updater){
			return updater(versions[r->current->version]);
		} else {
			versions[r->current->version] = value;
		}
	}
	return true;
}

template <class T, typename _Strategy>
bool Versioned<T,_Strategy>::SetMerge(std::shared_ptr<Revision> r, T& value){
	if (versions.find(r->current->version) == versions.end()) {
		r->current->written.push_back(this);
		versions[r->current->version] = value;
	} else {
		merge_strategy.merge(versions[r->current->version], value);
	}
	return true;
}

template <class T, typename _Strategy>
void Versioned<T,_Strategy>::Release(std::shared_ptr<Segment> release) {
	versions.erase(release->version);
}

template <class T, typename _Strategy>
void Versioned<T,_Strategy>::Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) {
    if (versions.find(main->current->version) == versions.end()) {
        Set(main, versions[parent->version]);
    }
    Release(parent);
}

template <class T, typename _Strategy>
void Versioned<T,_Strategy>::Merge(std::shared_ptr<Revision> main, std::shared_ptr<Revision> joinRev, std::shared_ptr<Segment> join) {
    std::shared_ptr<Segment> s = joinRev->current;
    while (versions.find(s->version) == versions.end()) {
        if (!s->parent)
            break;

        s = s->parent;
    }
    if (s == join) {
        SetMerge(main, versions[join->version]);
    }
}

#endif