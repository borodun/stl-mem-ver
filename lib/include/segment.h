#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <list>
#include <memory>

class Revision;
class VersionedI;

/**
 * @brief Class to keep track what Versioned variables were changed
 *
 */
class Segment : public std::enable_shared_from_this<Segment>
{
public:
	/**
	 * @brief Construct the very first Segment
	 *
	 */
	Segment();

	/**
	 * @brief Construct a new Segment object
	 *
	 * @param parent The parent Segment for new Segment
	 */
	Segment(std::shared_ptr<Segment> parent);

	/**
	 * @brief Release Segment when it's no longer needed
	 *
	 */
	void Release();

	/**
	 * @brief Compress branch of Segments into one Segment
	 *
	 * @param main Revision of whose branch of Segments needs to be collapsed
	 * @see Revision
	 */
	void Collapse(std::shared_ptr<Revision> main);

	/**
	 * @brief Previous Segment
	 *
	 */
	std::shared_ptr<Segment> parent;

	/**
	 * @brief Version of that Segment
	 *
	 */
	int version;

	/**
	 * @brief Count of references for that Segment
	 *
	 */
	int refcount;

	/**
	 * @brief List of all Versioned variables that were changed
	 *
	 * @see VersionedI
	 */
	std::list<VersionedI*> written;

private:
	/**
	 * @brief Last used Segment version number between all threads
	 *
	 */
	static int versionCount;
};

#endif