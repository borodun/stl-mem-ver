/**
 * @file  revision.h
 *
 * @brief Implementation of the class Revision.
 */

#ifndef __REVISION_H__
#define __REVISION_H__

#include <thread>
#include <memory>
#include <iostream>
#include <functional>

class Segment;

/**
 * @brief Revision class for for keeping track of segment branches
 *
 */
class Revision
{
public:
	/**
	 * @brief Construct for the very first Revision
	 *
	 */
	Revision();

	/**
	 * @brief Construct a new Revision for new thread
	 *
	 * @param root Segment from which new Revision was forked
	 * @param current Segemtn that will be the first for new Revision
	 * @see Segment
	 */
	Revision(std::shared_ptr<Segment> root, std::shared_ptr<Segment> current);

	/**
	 * @brief Segment from which this Revision was created
	 *
	 * @see Segment
	 */
	std::shared_ptr<Segment> root;

	/**
	 * @brief Current Segment of that Revision
	 *
	 * @see Segment
	 */
	std::shared_ptr<Segment> current;

	/**
	 * @brief Thread stuct to join that Revision after it is completed
	 *
	 */
	std::thread thread;

	/**
	 * @brief The current Revision for current thread
	 *
	 * It is static and thread_local so that for each thread only one
	 * Revision can exist
	 */
	thread_local static std::shared_ptr<Revision> currentRevision;
};

/**
 * @brief Print revision segments for debugging
 *
 */
void PrintRevision(std::shared_ptr<Revision> revision);

#endif