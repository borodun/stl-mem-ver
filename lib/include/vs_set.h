#ifndef _VS_SET_H
#define _VS_SET_H

#include <functional>
#include <set>
#include <initializer_list>

#include "versioned.h"
#include "revision.h"

namespace vs
{

		/* TODO: add param _Strategy */

	/**
	 *  @brief A mimic of a standard container made up of unique keys, which can be
	 *  retrieved in logarithmic time.
	 *
	 *
	 *  @param _Key  Type of key objects.
	 *  @param _Comp  Comparison function object type, defaults to less<_Key>.
	 */
	template<typename _Key, typename _Comp = std::less<_Key>>
	class vs_set
	{
	public:
	/* public typedefs */

	typedef Versioned<std::set<_Key, _Comp>> _Versioned;

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

	//*  @brief  Builds a %set from a range.
	//*  @brief  %Set copy constructor.
	//*  @brief %Set move constructor

	/* ------------------ Accesors ----------------------*/
	// /**
    //    *  Returns a read-only (constant) iterator that points to the first
    //    *  element in the %set.  Iteration is done in ascending order according
    //    *  to the keys.
    //    */


    //   /**
    //    *  Returns a read-only (constant) iterator that points one past the last
    //    *  element in the %set.  Iteration is done in ascending order according
    //    *  to the keys.
    //    */
	
	/* ------------------ Operators ----------------------*/

	// insert
	// = (copy)
	// = {}
	};
}

#endif
