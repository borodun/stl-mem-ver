#ifndef _VS_STACK_H
#define _VS_STACK_H

#include <functional>
#include <stack>
#include <initializer_list>
#include <iterator>

#include "versioned.h"
#include "revision.h"
#include "strategy.h"

namespace vs
{

	template<typename _Key>
	class vs_stack_strategy;

	/**
	 *  @brief A versioned mimic of a stl::stack, suitable for multithread
	 *
	 *  @param _Key  Type of key objects.
	 *  @param _Strategy  Custom strategy class for different merge behaviour
	 */
	template<typename _Key, typename _Strategy = vs_stack_strategy<_Key>>
	class vs_stack
	{

	static_assert(vs::IsMergeStrategy<_Strategy, std::stack<_Key>>, 
		"Provided invalid strategy class in template");

	public:
	/* public typedefs */

	typedef Versioned<std::stack<_Key>> _Versioned;
	typedef std::stack<_Key>::size_type size_type;

	private:

	_Versioned _v_s;

	public:

	/* ------------------ Constructors ----------------------*/
	/**
	 * @brief  Creates a vs_stack with no elements.
	 */
	explicit
	vs_stack()
	: _v_s(std::stack<_Key>()) { }

	/**
	 * @brief  Builds a vs_stack from an initializer_list.
	 * @param  __l  An initializer_list.
	 * @param  __comp  Comparator to use.
	 * 
	 * Copy elements of the list to created vs_stack.
	 * Non-standard, but handy to have instead of copying with stack{{initilizer}}
	 */
	vs_stack(std::initializer_list<_Key> __l)
	: _v_s(std::stack<_Key>()) 
	{
		_v_s.Set(_v_s.Get(),
		[&](std::stack<_Key>& _stack){
			for (auto& i: __l){
				_stack.push(i);
			}
			return true;
		});
	}

	/**
	 * @brief  vs_stack copy constructor
	 * 
	 * does not inherit versions history
	 */
	vs_stack(const vs_stack& __vs_stack)
	: _v_s(__vs_stack._v_s.Get()) { }

	//*  @brief %stack move constructor

	//*  @brief  Builds a %stack from a range.

	/* ------------------ Accessors ----------------------*/

	/**
	 * @brief access top element
	 * 
	 * Returns a read-only (constant) reference that points to the first
	 * element in the vs_stack.
	 */
	const _Key&
	top() const
	{ return _v_s.Get().top(); }

	/**
	 * @brief size of underlying stack
	 */
	size_type
	size() const noexcept
	{ return _v_s.Get().size(); }

	/* ------------------ Operators ----------------------*/

	/**
	 * @brief Attempts to push an element into the stack.
	 * @param  __x  Element to be inserted.
	 */
	void
	push(const _Key& __x)
	{
		_v_s.Set(_v_s.Get(), [&](std::stack<_Key>& _stack){ _stack.push(__x); return true; });
	}

	/**
	 * @brief Remove first element of stack
	 */
	void
	pop()
	{
		if (_v_s.Get().size() > 0)
			_v_s.Set(_v_s.Get(), [](std::stack<_Key>& _stack){ _stack.pop(); return true; });
	}
	// = (copy)
	// = {}
	};

	/**
	 * @brief simpliest determenistic merge strategy.
	 * 
	 * On merge, puts everything from one stack to other. It is expected to
	 * start with empty stacks and merge remainders, or for user to override
	 * this strategy.
	 */
	template<typename _Key>
	class vs_stack_strategy
	{
	public:

	void
	merge(std::stack<_Key>& dst, std::stack<_Key>& src)
	{
		while (src.size() > 0)
		{
			dst.push(src.top());
			src.pop();
		}
	}

	void
	merge_same_element(std::stack<_Key>& dst, _Key& dstk, _Key& srck)
	{
		dst.push(srck);
	}

	};
}

#endif
