#ifndef _VS_SET_H
#define _VS_SET_H

#include <functional>
#include <set>
#include <initializer_list>
#include <iterator>

#include "versioned.h"
#include "revision.h"
#include "strategy.h"

namespace vs
{

	template<typename _Key>
	class vs_set_strategy;

	/**
	 *  @brief A versioned mimic of a stl::set, suitable for multithread
	 *
	 *  @param _Key  Type of key objects.
	 *  @param _Comp  Comparison function object type, defaults to less<_Key>.
	 *  @param _Strategy  Custom strategy class for different merge behaviour
	 */
	template<typename _Key, typename _Comp = std::less<_Key>, 
		typename _Strategy = vs_set_strategy<_Key>>
	class vs_set
	{

	static_assert(vs::IsMergeStrategy<_Strategy, std::set<_Key>>, 
		"Provided invalid strategy class in template");

	public:
	/* public typedefs */

	typedef Versioned<std::set<_Key, _Comp>, _Strategy> _Versioned;
	typedef std::set<_Key, _Comp>::iterator iterator;
	typedef std::set<_Key, _Comp>::size_type size_type;

	private:

	_Versioned _v_s;

	public:

	/* ------------------ Constructors ----------------------*/
	/**
	 * @brief  Creates a vs_set with no elements.
	 * @param  __comp  Comparator to use.
	 */
	explicit
	vs_set(const _Comp& __comp = _Comp())
	: _v_s(std::set<_Key, _Comp>(__comp)) { }

	/**
	 * @brief  Builds a vs_set from an initializer_list.
	 * @param  __l  An initializer_list.
	 * @param  __comp  Comparator to use.
	 * 
	 * Copy elements of the list to created vs_set.
	 * Lots of copies in process, but only one version gets added.
	 */
	vs_set(std::initializer_list<_Key> __l,
		   const _Comp& __comp = _Comp())
	: _v_s(std::set<_Key, _Comp>(__l, __comp)) { }

	/**
	 * @brief  vs_set copy constructor
	 * 
	 * does not inherit versions history
	 */
	vs_set(const vs_set& __vs_set)
	: _v_s(__vs_set._v_s.Get()) { }

	//*  @brief %Set move constructor

	//*  @brief  Builds a %set from a range.


	/* ------------------ Accessors ----------------------*/

	/**
	 * @brief  begin constant iterator
	 * 
	 * Returns a read-only (constant) iterator that points to the first
	 * element in the vs_set.  Iteration is done in ascending order according
	 * to the keys.
	 */
	iterator
	begin() const noexcept
	{ return _v_s.Get().begin(); }

	/**
	 * @brief end constant iterator
	 * 
	 * Returns a read-only (constant) iterator that points one past the last
	 * element in the vs_set.  Iteration is done in ascending order according
	 * to the keys.
	 */
	iterator
	end() const noexcept
	{ return _v_s.Get().end(); }


	/**
	 * @brief size of underlying set
	 */
	size_type
	size() const noexcept
	{ return _v_s.Get().size(); }

	/**
	 * @brief check if element is contained in set
	 */
	bool
	contains(const _Key& __x)
	{ 
		auto& _set = _v_s.Get();
		return _set.find(__x) != _set.end();
	}

	/**
	 * @brief find element in set
	 */
	iterator
	find(const _Key& __x)
	{ return _v_s.Get().find(__x); }

	/* ------------------ Operators ----------------------*/

	/* XXX: add pair<iteraror as in stl> */
	/* XXX: move insert */

	/**
	 * @brief Attempts to insert an element into the %set.
	 * @param  __x  Element to be inserted.
	 * @return  true if element was inserted
	 *
	 * reasons not to insert:
	 * - this element is already present
	 */
	bool
	insert(const _Key& __x)
	{
		return _v_s.Set(_v_s.Get(), [&](std::set<_Key, _Comp>& _set){return _set.insert(__x).second;});
	}
	// = (copy)
	// = {}
	};

	/**
	 * @brief simpliest determenistic merge strategy.
	 * 
	 * On merge, puts everything from one set to other. It is expected to
	 * start with empty stacks and merge remainders, or for user to override
	 * this strategy.
	 * 
	 * Merge_same_element is empty, user is expected to override it for actually
	 * merging same elements.
	 */
	template<typename _Key>
	class vs_set_strategy
	{
	public:

	void
	merge(std::set<_Key>& dst, std::set<_Key>& src)
	{
		for (auto& i: src)
		{
			auto found = dst.find(i);
			/* XXX: dirty const_cast, but it is not used as const anyway */
			if (found != dst.end())
				merge_same_element(dst, const_cast<_Key&>(*found), const_cast<_Key&>(i));
			else
				dst.insert(i);
		}
	}

	void
	merge_same_element(std::set<_Key>& dst, _Key& dstk, _Key& srck)
	{
		/* do nothing, as insert would handle it */
	}

	};
}

#endif
