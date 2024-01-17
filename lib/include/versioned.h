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
};


// All methods need to be decalred in header due to template linking issues

template <class T>
Versioned<T>::Versioned(const T& v) {
    Set(Revision::currentRevision, v);
}

template <class T>
inline Versioned<T>::~Versioned()
{
	std::shared_ptr<Segment> s = Revision::currentRevision->current;

	while (s) {
        if (versions.find(s->version) != versions.end()) {
			for (std::list<VersionedI *>::iterator it = s->written.begin(); it != s->written.end(); ++it){
				if (*it == this) {
					s->written.erase(it++);
				}
			}
		}
        s = s->parent;
    }
}

template <class T>
const T& Versioned<T>::Get() const{
    return Get(Revision::currentRevision);
}

template <class T>
const T& Versioned<T>::Get(std::shared_ptr<Revision> r) const{
    std::shared_ptr<Segment> s = r->current;

    while (versions.find(s->version) == versions.end()) {
        if (!s->parent)
            break;

        s = s->parent;
    }

    return versions.at(s->version);
}

template <class T>
bool Versioned<T>::Set(const T& v, const std::function<bool(T&)>& updater) {
	return Set(Revision::currentRevision, v, updater);
}

template <class T>
bool Versioned<T>::Set(std::shared_ptr<Revision> r, const T& value, const std::function<bool(T&)>& updater) {
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

template <class T>
void Versioned<T>::Release(std::shared_ptr<Segment> release) {
	versions.erase(release->version);
}

template <class T>
void Versioned<T>::Collapse(std::shared_ptr<Revision> main, std::shared_ptr<Segment> parent) {
    if (versions.find(main->current->version) == versions.end()) {
        Set(main, versions[parent->version]);
    }
    Release(parent);
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

#endif