#ifndef _VS_QUEUE_H
#define _VS_QUEUE_H

#include <functional>
#include <queue>
#include <initializer_list>
#include <iterator>

#include "versioned.h"
#include "revision.h"

namespace vs
{

		/* TODO: add param _Strategy */

	/**
	 *  @brief A versioned mimic of a stl::queue, suitable for multithread
	 *
	 *  @param _Key  Type of key objects.
	 */
	template<typename _Key>
	class vs_queue
	{
	public:
	/* public typedefs */

	typedef Versioned<std::queue<_Key>> _Versioned;
	typedef std::queue<_Key>::size_type size_type;

	private:

	_Versioned _v_q;

	public:

	/* ------------------ Constructors ----------------------*/
	/**
	 * @brief  Creates a vs_queue with no elements.
	 */
	explicit
	vs_queue()
	: _v_q(std::queue<_Key>()) { }

	/**
	 * @brief  Builds a vs_queue from an initializer_list.
	 * @param  __l  An initializer_list.
	 * @param  __comp  Comparator to use.
	 * 
	 * Copy elements of the list to created vs_queue.
	 * Non-standard, but handy to have instead of copying with queue{{initilizer}}
	 */
	vs_queue(std::initializer_list<_Key> __l)
	: _v_q(std::queue<_Key>()) 
	{
		_v_q.Set(_v_q.Get(),
		[&](std::queue<_Key>& _queue){
			for (auto& i: __l){
				_queue.push(i);
			}
			return true;
		});
	}

	/**
	 * @brief  vs_queue copy constructor
	 * 
	 * does not inherit versions history
	 */
	vs_queue(const vs_queue& __vs_queue)
	: _v_q(__vs_queue._v_q.Get()) { }

	//*  @brief %queue move constructor

	//*  @brief  Builds a %queue from a range.

	/* ------------------ Accessors ----------------------*/

	/**
	 * @brief  begin constant reference
	 * 
	 * Returns a read-only (constant) reference that points to the first
	 * element in the vs_queue.
	 */
	const _Key&
	front() const
	{ return _v_q.Get().front(); }

	/**
	 * @brief end reference
	 * 
	 * Returns a reference to recently pushed element
	 */
	const _Key&
	back()
	{ return _v_q.Get().back(); }


	/**
	 * @brief size of underlying queue
	 */
	size_type
	size() const noexcept
	{ return _v_q.Get().size(); }

	/* ------------------ Operators ----------------------*/

	/**
	 * @brief Attempts to push an element into the queue.
	 * @param  __x  Element to be inserted.
	 */
	void
	push(const _Key& __x)
	{
		_v_q.Set(_v_q.Get(), [&](std::queue<_Key>& _queue){ _queue.push(__x); return true; });
	}

	/**
	 * @brief Remove first element of queue
	 */
	void
	pop()
	{
		if (_v_q.Get().size() > 0)
			_v_q.Set(_v_q.Get(), [](std::queue<_Key>& _queue){ _queue.pop(); return true; });
	}
	// = (copy)
	// = {}
	};
}

#endif
